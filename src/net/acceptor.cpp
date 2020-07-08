#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
acceptor::acceptor(const tcp::peer& endpoint, make_handler_func make_handler)
  : endpoint_(endpoint)
  , make_handler_(std::move(make_handler))
  , log_(matador::create_logger("Acceptor"))
{}

void acceptor::open()
{
  acceptor_.bind(endpoint_);
  acceptor_.listen(10);
  log_.info("fd %d: accepting connections", handle());
}

int acceptor::handle() const
{
  return acceptor_.id();
}

void acceptor::on_input()
{
  tcp::socket sock;
  log_.debug("fd %d: accepting connection ...", handle());
  acceptor_.accept(sock);

  // create new client handler
  auto h = make_handler_(sock, this);

  get_reactor()->register_handler(h, event_type::READ_WRITE_MASK);

  log_.debug("fd %d: accepted socket id %d", handle(), sock.id());
}

void acceptor::close()
{
  acceptor_.close();
  // Todo: unregister from reactor (maybe observer pattern?)
  // notify()
}

bool acceptor::is_ready_write() const
{
  return false;
}

bool acceptor::is_ready_read() const
{
  return handle() > 0;
}
}