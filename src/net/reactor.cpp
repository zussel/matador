#include "matador/net/reactor.hpp"
#include "matador/net/handler.hpp"

#include "matador/logger/log_manager.hpp"

#include <algorithm>
#include <limits>
#include <cerrno>
#include <ctime>

namespace matador {

reactor::reactor()
  : sentinel_(std::shared_ptr<handler>(nullptr))
  , log_(create_logger("Reactor"))
{

}
reactor::~reactor()
{
    log_.debug("destroying reactor");
}

void reactor::register_handler(const std::shared_ptr<handler>& h, event_type et)
{
  h->register_reactor(this);

  h->open();

  auto it = std::find_if(handlers_.begin(), handlers_.end(), [&h](const t_handler_type &val) {
    return val.first == h;
  });

  if (it == handlers_.end()) {
    handlers_.emplace_back(h, et);
  } else if (it->first != h) {
    throw std::logic_error("given handler isn't expected handler");
  } else {
    it->second = it->second | et;
  }
}

void reactor::unregister_handler(const std::shared_ptr<handler>& h, event_type)
{
  auto it = std::find_if(handlers_.begin(), handlers_.end(), [&h](const t_handler_type &ht) {
    return ht.first == h;
  });

  if (it != handlers_.end()) {
    (*it).first->close();
    handlers_.erase(it);
  }
}

void reactor::schedule_timer(const std::shared_ptr<handler>& h, time_t offset, time_t interval)
{
  h->register_reactor(this);

  auto it = std::find_if(handlers_.begin(), handlers_.end(), [&h](const t_handler_type &ht) {
    return ht.first == h;
  });

  if (it == handlers_.end()) {
    handlers_.emplace_back(h, event_type::NONE_MASK);
  }

  h->schedule(offset, interval);
}

void reactor::cancel_timer(const std::shared_ptr<handler>& h)
{
  auto it = std::find_if(handlers_.begin(), handlers_.end(), [&h](const t_handler_type &ht) {
    return ht.first == h;
  });

  if (it != handlers_.end()) {
    handlers_.erase(it);
  }

  h->cancel_timer();
}

void reactor::run()
{
  log_.info("starting reactor");

  running_ = true;
  time_t timeout;
  while (running_) {
    prepare_select_bits(timeout);

    log_.debug("fds [r: %d, w: %d, e: %d]",
              fdsets_.read_set().count(),
              fdsets_.write_set().count(),
              fdsets_.except_set().count());

    if (timeout != (std::numeric_limits<time_t>::max)()) {
      log_.debug("next timeout in %d sec", timeout);
    }
    struct timeval tselect{};
    struct timeval* p = nullptr;
    if (timeout < (std::numeric_limits<time_t>::max)()) {
      tselect.tv_sec = timeout;
      tselect.tv_usec = 0;
      p = &tselect;
    }

    if (fdsets_.maxp1() < 1 && timeout == (std::numeric_limits<time_t>::max)()) {
      log_.info("no clients to handle, exiting");
      return;
    }

    int ret;
    while ((ret = select(p)) < 0) {
      if(errno != EINTR) {
        char error_buffer[1024];
        log_.warn("select failed: %s", os::strerror(errno, error_buffer, 1024));
        shutdown();
      } else {
        return;
      }
    }

    bool interrupted = is_interrupted();

    if (interrupted) {
      cleanup();
    } else {
      process_handler(ret);
      remove_deleted();
    }

    if (handlers_.empty() && timeout == (std::numeric_limits<time_t>::max)()) {
      log_.info("no clients to handle, exiting");
      running_ = false;
    }
  }

  cleanup();
}

void reactor::shutdown()
{
  // shutdown the reactor properly
  log_.info("shutting down reactor");
  shutdown_requested_ = true;
  interrupter_.interrupt();
}

bool reactor::is_running() const
{
  return running_;
}

void reactor::prepare_select_bits(time_t& timeout)
{
  fdsets_.reset();
  time_t now = ::time(nullptr);
  timeout = (std::numeric_limits<time_t>::max)();

  // set interrupter fd
  fdsets_.read_set().set(interrupter_.socket_id());

  for (const auto &h : handlers_) {
    if (h.first == nullptr) {
      continue;
    }
    if (h.first->is_ready_read() && is_event_type_set(h.second, event_type::READ_MASK)) {
      fdsets_.read_set().set(h.first->handle());
    }
    if (h.first->is_ready_write() && is_event_type_set(h.second, event_type::WRITE_MASK)) {
      fdsets_.write_set().set(h.first->handle());
    }
    if (h.first->next_timeout() > 0) {
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

int reactor::select(struct timeval *timeout)
{
  log_.debug("calling select; waiting for io events");
  return ::select(
    fdsets_.maxp1() + 1,
    fdsets_.read_set().get(),
    fdsets_.write_set().get(),
    fdsets_.except_set().get(),
    timeout
  );
}

void reactor::process_handler(int /*ret*/)
{
  handlers_.emplace_back(sentinel_, event_type::NONE_MASK);
  time_t now = ::time(nullptr);
  while (handlers_.front().first != nullptr) {
    auto h = handlers_.front();
    handlers_.pop_front();
    handlers_.push_back(h);
    // check for read/accept
    if (h.first->handle() > 0 && fdsets_.write_set().is_set(h.first->handle())) {
      on_write_mask(h.first);
    }
    if (h.first->handle() > 0 && fdsets_.read_set().is_set(h.first->handle())) {
      on_read_mask(h.first);
    }
    if (h.first->next_timeout() > 0 && h.first->next_timeout() <= now) {
      on_timeout(h.first, now);
    }
//    handlers_to_delete_.clear();
  }
  handlers_.pop_front();
}

void reactor::on_read_mask(const std::shared_ptr<handler>& h)
{
  log_.debug("read bit for handler %d is set; handle input", h->handle());
  h->on_input();
}

void reactor::on_write_mask(const std::shared_ptr<handler>& h)
{
  log_.debug("write bit for handler %d is set; handle output", h->handle());
  h->on_output();
}

void reactor::on_except_mask(const std::shared_ptr<handler>&)
{

}

void reactor::on_timeout(const std::shared_ptr<handler> &h, time_t now)
{
  log_.debug("timeout expired for handler %d; handle timeout", h->handle());
  h->calculate_next_timeout(now);
  h->on_timeout();
}

const select_fdsets &reactor::fdsets() const
{
  return fdsets_;
}

void reactor::mark_handler_for_delete(const std::shared_ptr<handler>& h)
{
  handlers_to_delete_.push_back(h);
}

bool reactor::is_interrupted()
{
  if (fdsets_.read_set().is_set(interrupter_.socket_id())) {
    log_.debug("interrupt byte received; resetting interrupter");
    if (shutdown_requested_) {
      running_ = false;
    }
    return interrupter_.reset();
  }
  return false;
}

}
