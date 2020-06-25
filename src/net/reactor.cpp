#include "matador/net/reactor.hpp"
#include "matador/net/handler.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>

namespace matador {

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
//  std::cout << "fds [r: "
//            << fdsets_.read_set().count() << ", w: "
//            << fdsets_.write_set().count() << ", e:"
//            << fdsets_.except_set().count() << "]\n";

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
          //std::cout << "select failed: " << strerror(errno) << "\n";
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
  for (auto h : handlers_) {
    if (h->is_ready_read()) {
      fdsets_.read_set().set(h->handle());
    }
  }
}

void reactor::cleanup()
{

}

void reactor::process_handler(int)
{
  for (auto h : handlers_) {
    // check for read/accept
    if (h->handle() > 0 && fdsets_.read_set().is_set(h->handle())) {
      h->on_input();
    }
    if (h->handle() > 0 && fdsets_.write_set().is_set(h->handle())) {
      h->on_output();
    }
  }
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

}
