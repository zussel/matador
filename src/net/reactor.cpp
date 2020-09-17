#include "matador/net/reactor.hpp"
#include "matador/net/handler.hpp"

#include "matador/logger/log_manager.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <ctime>

namespace matador {

reactor::reactor()
  : sentinel_(std::shared_ptr<handler>(nullptr))
  , log_(create_logger("Reactor"))
{

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

    log_.info("fds [r: %d, w: %d, e: %d]",
              fdsets_.read_set().count(),
              fdsets_.write_set().count(),
              fdsets_.except_set().count());

    if (timeout != (std::numeric_limits<time_t>::max)()) {
      log_.info("next timeout in %d sec", timeout);
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
        log_.warn("select failed: %s", strerror(errno));
        shutdown();
      } else {
        return;
      }
    }

    process_handler(ret);
    remove_deleted();
  }

  cleanup();
}

void reactor::shutdown()
{
  // shutdown the reactor properly
  running_ = false;
}

void reactor::prepare_select_bits(time_t& timeout)
{
  fdsets_.reset();
  time_t now = ::time(nullptr);
  timeout = (std::numeric_limits<time_t>::max)();
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

void reactor::remove_deleted() {

}

void reactor::cleanup()
{

}

int reactor::select(struct timeval *timeout)
{
  return ::select(
    fdsets_.maxp1() + 1,
    fdsets_.read_set().get(),
    fdsets_.write_set().get(),
    fdsets_.except_set().get(),
    timeout
  );
}

void reactor::process_handler(int ret)
{
  log_.info("process %d handlers", ret);
  handlers_.emplace_back(sentinel_, event_type::NONE_MASK);
  time_t now = ::time(nullptr);
  while (handlers_.front().first != nullptr) {
    auto h = handlers_.front();
    handlers_.pop_front();
    handlers_.push_back(h);
    // check for read/accept
    if (h.first->handle() > 0 && fdsets_.write_set().is_set(h.first->handle())) {
      h.first->on_output();
    }
    if (h.first->handle() > 0 && fdsets_.read_set().is_set(h.first->handle())) {
      h.first->on_input();
    }
    if (h.first->next_timeout() > 0 && h.first->next_timeout() <= now) {
      h.first->calculate_next_timeout(now);
      h.first->on_timeout();
    }
    handlers_to_delete_.clear();
  }
  handlers_.pop_front();
}

void reactor::on_read_mask()
{

}

void reactor::on_write_mask()
{

}

void reactor::on_except_mask()
{

}

const select_fdsets &reactor::fdsets() const
{
  return fdsets_;
}

void reactor::mark_handler_for_delete(const std::shared_ptr<handler>& h)
{
  handlers_to_delete_.push_back(h);
}
}
