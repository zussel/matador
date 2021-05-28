#include <utility>
#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {

acceptor::acceptor()
  : log_(matador::create_logger("Acceptor"))
{}

acceptor::acceptor(tcp::peer endpoint)
: endpoint_(std::move(endpoint))
, log_(matador::create_logger("Acceptor"))
{}

acceptor::acceptor(tcp::peer  endpoint, t_accept_handler make_handler)
  : endpoint_(std::move(endpoint))
  , accept_handler_(std::move(make_handler))
  , log_(matador::create_logger("Acceptor"))
{}

acceptor::~acceptor()
{
  acceptor_.close();
}

void acceptor::accecpt(acceptor::t_accept_handler on_new_connection)
{
  accept_handler_ = std::move(on_new_connection);
}

void acceptor::accecpt(const tcp::peer &endpoint, acceptor::t_accept_handler on_new_connection)
{
  endpoint_ = endpoint;
  accecpt(std::move(on_new_connection));
}

void acceptor::open()
{
  acceptor_.bind(endpoint_);
  acceptor_.listen(10);
  log_.debug("fd %d: accepting connections", handle());
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
  int ret = acceptor_.accept(sock, endpoint);

  if (ret < 0) {
    char error_buffer[1024];
    os::strerror(errno, error_buffer, 1024);
    log_.error("accept failed: %s", error_buffer);
  } else {
    // create new client handler
    log_.debug("accepted connection from %s", endpoint.to_string().c_str());

    auto h = accept_handler_(sock, endpoint, this);

    get_reactor()->register_handler(h, event_type::READ_WRITE_MASK);

    log_.debug("fd %d: accepted socket id %d", handle(), sock.id());
  }
}

void acceptor::close()
{
  log_.debug("closing acceptor %d", acceptor_.id());
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

const tcp::peer &acceptor::endpoint() const
{
  return endpoint_;
}

tcp::peer acceptor::create_client_endpoint() const
{
  if (endpoint_.addr().is_v4()) {
    return matador::tcp::peer(address::v4::empty());
  } else {
    return matador::tcp::peer(address::v6::empty());
  }
}

void acceptor::notify_close(handler *)
{

}

std::string acceptor::name() const
{
  return "acceptor";
}

}