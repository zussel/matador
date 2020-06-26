#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
acceptor::acceptor(make_handler_func make_handler)
  : make_handler_(std::move(make_handler))
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
  auto h = make_handler_(sock, this);

  h->open();

  get_reactor()->register_handler(h, event_type::READ_WRITE_MASK);
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