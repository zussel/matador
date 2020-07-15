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

void reactor::register_handler(const std::shared_ptr<handler>& h, event_type)
{
  h->open();

  h->register_reactor(this);

  auto it = std::find(handlers_.begin(), handlers_.end(), h);

  if (it == handlers_.end()) {
    handlers_.push_back(h);
  }
}

void reactor::unregister_handler(const std::shared_ptr<handler>& h, event_type)
{
  auto it = std::find(handlers_.begin(), handlers_.end(), h);

  if (it != handlers_.end()) {
    (*it)->close();
    handlers_.erase(it);
  }
}

void reactor::schedule_timer(const std::shared_ptr<handler>& h, time_t offset, time_t interval)
{
  auto it = std::find(handlers_.begin(), handlers_.end(), h);

  if (it == handlers_.end()) {
    handlers_.push_back(h);
  }

  h->schedule(offset, interval);
}

void reactor::cancel_timer(const std::shared_ptr<handler>& h)
{
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

    log_.info("next timeout in %d sec", timeout);
    struct timeval tselect{};
    struct timeval* p = nullptr;
    if (timeout < (std::numeric_limits<time_t>::max)()) {
      tselect.tv_sec = timeout;
      tselect.tv_usec = 0;
      p = &tselect;
    }

    if (fdsets_.maxp1() < 1) {
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
    if (h == nullptr) {
      continue;
    }
    if (h->is_ready_read()) {
      fdsets_.read_set().set(h->handle());
    }
    if (h->is_ready_write()) {
      fdsets_.write_set().set(h->handle());
    }
    if (h->next_timeout() > 0) {
      timeout = (std::min)(timeout, h->next_timeout() <= now ? 0 : (h->next_timeout() - now));
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
  handlers_.push_back(sentinel_);
  time_t now = ::time(nullptr);
  while (handlers_.front().get() != nullptr) {
    auto h = handlers_.front();
    handlers_.pop_front();
    handlers_.push_back(h);
    // check for read/accept
    if (h->handle() > 0 && fdsets_.write_set().is_set(h->handle())) {
      h->on_output();
    }
    if (h->handle() > 0 && fdsets_.read_set().is_set(h->handle())) {
      h->on_input();
    }
    if (h->next_timeout() > 0 && h->next_timeout() <= now) {
      h->calculate_next_timeout(now);
      h->on_timeout();
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
