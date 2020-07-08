#include "matador/net/reactor.hpp"
#include "matador/net/handler.hpp"

#include "matador/logger/log_manager.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>

namespace matador {

reactor::reactor()
  : sentinel_(std::shared_ptr<handler>(nullptr))
  , log_(create_logger("reactor"))
{

}

void reactor::register_handler(std::shared_ptr<handler> h, event_type)
{
  h->open();

  h->register_reactor(this);

  auto it = std::find(handlers_.begin(), handlers_.end(), h);

  if (it == handlers_.end()) {
    handlers_.push_back(h);
  }
}

void reactor::unregister_handler(std::shared_ptr<handler> h, event_type)
{
  auto it = std::find(handlers_.begin(), handlers_.end(), h);

  if (it != handlers_.end()) {
    (*it)->close();
    handlers_.erase(it);
  }
}

void reactor::run()
{
  log_.info("starting reactor");
  log_.info("fds [r: %d, w: %d, e: %d]",
    fdsets_.read_set().count(),
    fdsets_.write_set().count(),
    fdsets_.except_set().count());

  running_ = true;
  while (running_) {
    prepare_fdsets();

    if (fdsets_.max() < 1) {
      return;
    }

    int ret = ::select(
      fdsets_.max() + 1,
      fdsets_.read_set().get(),
      fdsets_.write_set().get(),
      fdsets_.except_set().get(),
      nullptr
    );

    switch (ret) {
      case -1:
        if (errno != EINTR) {
          log_.error("select failed: %s", strerror(errno));
          shutdown();
        } else {
          continue;
        }
        break;
      case 0:
        // check timeout
        process_timeout();
        break;
      default:
        // process handler
        process_handler(ret);
        break;
    }
  }

  cleanup();
}

void reactor::shutdown()
{
  // shutdown the reactor properly
  running_ = false;
}

void reactor::prepare_fdsets()
{
  fdsets_.reset();
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
  }
}

void reactor::cleanup()
{

}

void reactor::process_handler(int)
{
  handlers_.push_back(sentinel_);
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
    handlers_to_delete_.clear();
  }
  handlers_.pop_front();
}

void reactor::process_timeout()
{

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
