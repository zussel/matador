#include "matador/net/reactor.hpp"
#include "matador/net/handler.hpp"

#include "matador/logger/log_manager.hpp"

#include <algorithm>
#include <limits>
#include <cerrno>
#include <ctime>
#include <iostream>

namespace matador {

reactor::reactor()
  : sentinel_(std::shared_ptr<handler>(nullptr))
  , log_(create_logger("Reactor"))
  , thread_pool_(4, [this]() { handle_events(); })
{
}
reactor::~reactor()
{
    log_.debug("destroying reactor");
}

void reactor::register_handler(const handler_ptr& h, event_type et)
{
  h->register_reactor(this);
  h->open();

  std::lock_guard<std::mutex> l(mutex_);
  auto it = find_handler_type(h);

  if (it == handlers_.end()) {
    handlers_.emplace_back(h, et);
  } else if (it->first != h) {
    throw std::logic_error("given handler isn't expected handler");
  } else {
    it->second = it->second | et;
  }
  interrupt_without_lock();
}

void reactor::unregister_handler(const handler_ptr& h, event_type)
{
  std::lock_guard<std::mutex> l(mutex_);
  auto it = find_handler_type(h);

  if (it != handlers_.end()) {
    (*it).first->close();
    handlers_.erase(it);
  }
  interrupt_without_lock();
}

void reactor::schedule_timer(const std::shared_ptr<handler>& h, time_t offset, time_t interval)
{
  h->register_reactor(this);

  std::lock_guard<std::mutex> l(mutex_);
  auto it = find_handler_type(h);

  if (it == handlers_.end()) {
    handlers_.emplace_back(h, event_type::TIMEOUT_MASK);
  }

  h->schedule(offset, interval);
}

void reactor::cancel_timer(const std::shared_ptr<handler>& h)
{
  std::lock_guard<std::mutex> l(mutex_);
  auto it = find_handler_type(h);

  if (it != handlers_.end()) {
    handlers_.erase(it);
  }

  h->cancel_timer();
}

void reactor::run()
{
//  log_.info("start dispatching all clients");
  thread_pool_.start();

  {
//    log_.info("waiting for reactor shutdown");
    std::unique_lock<std::mutex> l(mutex_);
    shutdown_.wait(l, [this]() { return shutdown_requested_.load(); });
    cleanup();
  }
//  log_.info("all clients dispatched; shutting down");
  thread_pool_.shutdown();
}

void reactor::handle_events()
{
//  log_.info("handle events");

  running_ = true;
  time_t timeout;
  select_fdsets fd_sets;
  prepare_select_bits(timeout, fd_sets);

//  log_.debug("fds [r: %d, w: %d, e: %d]",
//             fdsets_.read_set().count(),
//             fdsets_.write_set().count(),
//             fdsets_.except_set().count());

//  if (timeout != (std::numeric_limits<time_t>::max)()) {
//    log_.debug("next timeout in %d sec", timeout);
//  }
  struct timeval tselect{};
  struct timeval* p = nullptr;
  if (timeout < (std::numeric_limits<time_t>::max)()) {
    tselect.tv_sec = timeout;
    tselect.tv_usec = 0;
    p = &tselect;
    std::cout << this << " next timeout in " << p->tv_sec << " seconds\n" << std::flush;
  }

  if (!has_clients_to_handle(timeout, fd_sets)) {
//    log_.info("no clients to handle, exiting");
    return;
  }

  int ret;
  while ((ret = select(p, fd_sets)) < 0) {
    std::cout << this << " select returned with error " << ret << "\n" << std::flush;
    if(errno != EINTR) {
      char error_buffer[1024];
      log_.warn("select failed: %s", os::strerror(errno, error_buffer, 1024));
      shutdown();
    } else {
      return;
    }
  }

  std::cout << this << " select returned with active requests " << ret << "\n" << std::flush;

  bool interrupted = is_interrupted(fd_sets);

  if (interrupted) {
    if (!shutdown_requested_) {
      std::cout << this << " reactor was interrupted - promote new leader\n" << std::flush;
//      log_.info("reactor was interrupted");
      thread_pool_.promote_new_leader();
      return;
    } else {
      std::cout << this << " reactor was interrupted for shutdown\n" << std::flush;
//      log_.info("shutting down");
//      cleanup();
      shutdown_.notify_one();
      return;
    }
  }

  time_t now = ::time(nullptr);
  t_handler_type handler_type = resolve_next_handler(now, fd_sets);

  if (handler_type.first) {
    deactivate_handler(handler_type.first, handler_type.second);
    std::cout << this << " handling client " << handler_type.first->name() << " (socket: " << handler_type.first->handle() << ") - promoting new leader\n" << std::flush;

    thread_pool_.promote_new_leader();
//    log_.info("start handling event");
    // handle event
    if (handler_type.second == event_type::WRITE_MASK) {
      on_write_mask(handler_type.first);
    } else if (handler_type.second == event_type::READ_MASK) {
      on_read_mask(handler_type.first);
    } else if (handler_type.second == event_type::TIMEOUT_MASK) {
      on_timeout(handler_type.first, now);
    } else {
//      log_.info("unknown event type");
    }
    activate_handler(handler_type.first, handler_type.second);
    remove_deleted();
  } else {
    // no handler found
//    log_.info("no handler found");
    thread_pool_.promote_new_leader();
  }
}

void reactor::shutdown()
{
  if (!is_running()) {
    return;
  }
  // shutdown the reactor properly
  log_.info("shutting down reactor");
  shutdown_requested_ = true;
  interrupt();
}

bool reactor::is_running() const
{
  return running_;
}

void reactor::prepare_select_bits(time_t& timeout, select_fdsets& fd_sets) const
{
  std::lock_guard<std::mutex> l(mutex_);
  fd_sets.reset();
  time_t now = ::time(nullptr);
  timeout = (std::numeric_limits<time_t>::max)();

  // set interrupter fd
  fd_sets.read_set().set(interrupter_.socket_id());

  for (const auto &h : handlers_) {
    if (h.first == nullptr) {
      continue;
    }
    if (h.first->is_ready_read() && is_event_type_set(h.second, event_type::READ_MASK)) {
      fd_sets.read_set().set(h.first->handle());
    }
    if (h.first->is_ready_write() && is_event_type_set(h.second, event_type::WRITE_MASK)) {
      fd_sets.write_set().set(h.first->handle());
    }
    if (h.first->next_timeout() > 0 && is_event_type_set(h.second, event_type::TIMEOUT_MASK)) {
      timeout = (std::min)(timeout, h.first->next_timeout() <= now ? 0 : (h.first->next_timeout() - now));
    }
  }
}

void reactor::remove_deleted()
{
  while (!handlers_to_delete_.empty()) {
    auto h = handlers_to_delete_.front();
    handlers_to_delete_.pop_front();
    auto fi = std::find_if(handlers_.begin(), handlers_.end(), [&h](const t_handler_type &ht) {
      return ht.first.get() == h.get();
    });

    if (fi != handlers_.end()) {
      log_.debug("removing handler %d", fi->first->handle());
      handlers_.erase(fi);
    }
  }
}

void reactor::cleanup()
{
  while (!handlers_.empty()) {
    auto hndlr = handlers_.front();
    handlers_.pop_front();
    hndlr.first->close();
  }
}

int reactor::select(struct timeval *timeout, select_fdsets& fd_sets)
{
  log_.debug("calling select; waiting for io events");
  return ::select(
    fd_sets.maxp1() + 1,
    fd_sets.read_set().get(),
    fd_sets.write_set().get(),
    fd_sets.except_set().get(),
    timeout
  );
}


//void reactor::process_handler(int /*ret*/)
//{
//  handlers_.emplace_back(sentinel_, event_type::NONE_MASK);
//  time_t now = ::time(nullptr);
//  while (handlers_.front().first != nullptr) {
//    auto h = handlers_.front();
//    handlers_.pop_front();
//    handlers_.push_back(h);
//    // check for read/accept
//    if (h.first->handle() > 0 && fdsets_.write_set().is_set(h.first->handle())) {
//      on_write_mask(h.first);
//    }
//    if (h.first->handle() > 0 && fdsets_.read_set().is_set(h.first->handle())) {
//      on_read_mask(h.first);
//    }
//    if (h.first->next_timeout() > 0 && h.first->next_timeout() <= now) {
//      on_timeout(h.first, now);
//    }
//  }
//  handlers_.pop_front();
//}

reactor::t_handler_type reactor::resolve_next_handler(time_t now, select_fdsets& fd_sets)
{
  std::lock_guard<std::mutex> l(mutex_);
  for (auto &h : handlers_) {
    if (h.first->handle() > 0 && fd_sets.write_set().is_set(h.first->handle())) {
      return std::make_pair(h.first, event_type::WRITE_MASK);
    }
    if (h.first->handle() > 0 && fd_sets.read_set().is_set(h.first->handle())) {
      return std::make_pair(h.first, event_type::READ_MASK);
    }
    if (h.first->next_timeout() > 0 && h.first->next_timeout() <= now) {
      return std::make_pair(h.first, event_type::TIMEOUT_MASK);
    }
  }
  return std::make_pair(nullptr, event_type::NONE_MASK);
}

void reactor::on_read_mask(const handler_ptr& h)
{
//  log_.debug("read bit for handler %d is set; handle input", h->handle());
  std::cout << this << " (handler " << h.get() << "): handle read\n" << std::flush;
  h->on_input();
}

void reactor::on_write_mask(const handler_ptr& h)
{
//  log_.debug("write bit for handler %d is set; handle output", h->handle());
  std::cout << this << " (handler " << h.get() << "): handle write\n" << std::flush;
  h->on_output();
}

void reactor::on_except_mask(const handler_ptr& h)
{
  std::cout << this << " (handler " << h.get() << "): handle exception\n" << std::flush;

}

void reactor::on_timeout(const handler_ptr &h, time_t now)
{
//  log_.debug("timeout expired for handler %d; handle timeout", h->handle());
  std::cout << this << " (handler " << h.get() << "): handle timeout\n" << std::flush;
  h->calculate_next_timeout(now);
  h->on_timeout();
}

select_fdsets reactor::fdsets() const
{
  time_t timeout;
  select_fdsets fd_sets;
  prepare_select_bits(timeout, fd_sets);
  return fd_sets;
}

void reactor::mark_handler_for_delete(const handler_ptr& h)
{
  std::lock_guard<std::mutex> l(mutex_);
  handlers_to_delete_.push_back(h);
}

bool reactor::is_interrupted(select_fdsets& fd_sets)
{
  std::lock_guard<std::mutex> l(mutex_);
  if (fd_sets.read_set().is_set(interrupter_.socket_id())) {
    log_.debug("interrupt byte received; resetting interrupter");
    if (shutdown_requested_) {
      running_ = false;
    }
    return interrupter_.reset();
  }
  return false;
}

bool reactor::has_clients_to_handle(time_t timeout, select_fdsets& fd_sets) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return fd_sets.maxp1() > 0 || timeout != (std::numeric_limits<time_t>::max)();
}

std::list<reactor::t_handler_type>::iterator reactor::find_handler_type(const reactor::handler_ptr &h)
{
  return std::find_if(handlers_.begin(), handlers_.end(), [&h](const t_handler_type &ht) {
    return ht.first.get() == h.get();
  });
}

void reactor::activate_handler(const reactor::handler_ptr &h, event_type ev)
{
  std::lock_guard<std::mutex> l(mutex_);
  auto it = find_handler_type(h);
  if (it == handlers_.end()) {
    return;
  }
  it->second |= ev;
}

void reactor::deactivate_handler(const reactor::handler_ptr &h, event_type ev)
{
  std::lock_guard<std::mutex> l(mutex_);
  auto it = find_handler_type(h);
  if (it == handlers_.end()) {
    return;
  }
  it->second &= ~ev;
}

void reactor::interrupt()
{
  log_.info("interrupting reactor");
  std::lock_guard<std::mutex> l(mutex_);
  interrupter_.interrupt();
}

void reactor::interrupt_without_lock()
{
  log_.info("interrupting reactor");
  interrupter_.interrupt();
}

}
