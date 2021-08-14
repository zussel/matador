#include "matador/logger/log_manager.hpp"

#include "matador/net/connector.hpp"
#include "matador/net/reactor.hpp"

#include "matador/utils/os.hpp"

#include <utility>

namespace matador {

connector::connector()
  : log_(matador::create_logger("Connector"))
{}

connector::connector(t_connect_handler on_new_connection)
  : connect_handler_(std::move(on_new_connection))
  , log_(matador::create_logger("Connector"))
{}

void connector::connect(reactor &r, const std::vector<tcp::peer> &endpoints)
{
  endpoints_ = endpoints;
  r.schedule_timer(shared_from_this(), 0, 3);
}

void connector::connect(reactor &r, const std::vector<tcp::peer> &endpoints, t_connect_handler on_new_connection)
{
  connect_handler_ = std::move(on_new_connection);
  connect(r, endpoints);
}

int connector::handle() const
{
  return 0;
}

void connector::on_timeout()
{
  tcp::socket stream;
  for (const auto &ep : endpoints_) {
    int ret = matador::connect(stream, ep);
    if (ret != 0) {
	  char error_buffer[1024];
      log_.error("couldn't establish connection to: %s", ep.to_string().c_str(), os::strerror(errno, error_buffer, 1024));
      continue;
    } else {
      log_.info("connection established to %s (fd: %d)", ep.to_string().c_str(), stream.id());
    }

    stream.non_blocking(true);
    auto h = connect_handler_(stream, ep, this);

    get_reactor()->register_handler(h, event_type::READ_WRITE_MASK);
    break;
  }
  if (stream.is_open()) {
    endpoints_.clear();
    get_reactor()->cancel_timer(shared_from_this());
  }
}

bool connector::is_ready_write() const
{
  return false;
}

bool connector::is_ready_read() const
{
  return false;
}

void connector::notify_close(handler *)
{

}

std::string connector::name() const
{
  return "connector";
}

}