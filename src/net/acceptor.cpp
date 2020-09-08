#include <iostream>
#include <utility>
#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {

acceptor::acceptor()
  : log_(matador::create_logger("Acceptor"))
{

}

acceptor::acceptor(const tcp::peer& endpoint, make_handler_func make_handler)
  : endpoint_(endpoint)
  , make_handler_(std::move(make_handler))
  , log_(matador::create_logger("Acceptor"))
{}

void acceptor::accecpt(const tcp::peer &endpoint, acceptor::make_handler_func on_new_connection)
{
  endpoint_ = endpoint;
  make_handler_ = std::move(on_new_connection);
}

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

  tcp::peer endpoint = create_client_endpoint();
  log_.debug("fd %d: accepting connection ...", handle());
  acceptor_.accept(sock, endpoint);

  // create new client handler
  log_.info("connection from %s", endpoint.to_string().c_str());

  auto h = make_handler_(sock, endpoint, this);

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

tcp::peer acceptor::create_client_endpoint() const
{
  if (endpoint_.addr().is_v4()) {
    return matador::tcp::peer(address::v4::empty());
  } else {
    return matador::tcp::peer(address::v6::empty());
  }
}

}