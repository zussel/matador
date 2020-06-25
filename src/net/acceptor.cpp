#include "matador/net/acceptor.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
acceptor::acceptor(std::function<handler *()> on_new_connection)
  : on_new_connection_(std::move(on_new_connection))
  , log_(matador::create_logger("Acceptor"))
{}

acceptor::acceptor(const char *hostname, unsigned short port, std::function<handler*()> on_new_connection)
  : acceptor_(hostname, port)
  , on_new_connection_(std::move(on_new_connection))
  , log_(matador::create_logger("Acceptor"))
{}

acceptor::acceptor(tcp::peer peer, std::function<handler*()> on_new_connection)
  : acceptor_(peer)
  , on_new_connection_(std::move(on_new_connection))
  , log_(matador::create_logger("Acceptor"))
{}

void acceptor::open()
{
  acceptor_.listen(10);
}

int acceptor::handle() const
{
  return acceptor_.id();
}

void acceptor::on_input()
{
  tcp::socket sock;
  log_.debug("accepting connection ...");
  acceptor_.accept(sock);

  // create new client handler
  on_new_connection_();
  log_.debug("accepted socket id %d", sock.id());
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