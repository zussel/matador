#ifndef MATADOR_IO_SERVICE_HPP
#define MATADOR_IO_SERVICE_HPP

#include "matador/utils/buffer.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"
#include "matador/net/stream_handler.hpp"

namespace matador {

class io_service
{
public:
  io_service();

  void run();

  template < typename AcceptCallback >
  void accept(const std::shared_ptr<acceptor>& ac, const tcp::peer &ep, AcceptCallback accept_callback);
  template < typename AcceptCallback >
  void accept(const std::shared_ptr<acceptor>& ac, AcceptCallback accept_callback);

  template < typename ConnectCallback >
  void connect(const std::shared_ptr<connector>& co, const std::string &port, ConnectCallback connect_callback);

private:
  logger log_;
  reactor reactor_;
};

template<typename AcceptCallback>
void io_service::accept(const std::shared_ptr<acceptor>& ac, const tcp::peer &ep, AcceptCallback accept_callback)
{
  log_.info("registering acceptor for %s", ep.to_string().c_str());
  ac->accecpt(ep, [accept_callback, this](tcp::socket sock, tcp::peer p, acceptor *accptr) {
    return std::make_shared<stream_handler>(sock, p, accptr, accept_callback);
  });

  reactor_.register_handler(ac, event_type::ACCEPT_MASK);
}

template<typename AcceptCallback>
void io_service::accept(const std::shared_ptr<acceptor>& ac, AcceptCallback accept_callback)
{
  log_.info("registering acceptor for %s", ac->endpoint().to_string().c_str());
  ac->accecpt([accept_callback, this](tcp::socket sock, tcp::peer p, acceptor *accptr) {
    return std::make_shared<stream_handler>(sock, p, accptr, accept_callback);
  });

  reactor_.register_handler(ac, event_type::ACCEPT_MASK);
}

template<typename ConnectCallback>
void io_service::connect(const std::shared_ptr<connector> &co, const std::string &port, ConnectCallback connect_callback)
{
  log_.info("registering connector for localhost:%s", port.c_str());
  tcp::resolver resolver;
  auto endpoints = resolver.resolve("localhost", port);
  co->connect(reactor_, endpoints, [connect_callback, this](const tcp::socket& sock, const tcp::peer &p, connector *cnnctr) {
    return std::make_shared<stream_handler>(sock, p, cnnctr, connect_callback);
  });
}

}
#endif //MATADOR_IO_SERVICE_HPP
