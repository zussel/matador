#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"
#include "matador/net/error.hpp"

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

  tcp::peer endpoint = create_client_endpoint();
  log_.debug("fd %d: accepting connection ...", handle());
  int fd = acceptor_.accept(endpoint);

  if (fd > 0) {
    sock.assign(fd);
    sock.non_blocking(true);
    sock.cloexec(true);
  } else {
    detail::throw_logic_error_with_errno("accept failed: ", errno);
  }

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

tcp::peer acceptor::create_client_endpoint() const
{
  if (endpoint_.addr().is_v4()) {
    return matador::tcp::peer(address::v4::empty());
  } else {
    return matador::tcp::peer(address::v6::empty());
  }
}

}