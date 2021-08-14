#ifndef MATADOR_IO_SERVICE_HPP
#define MATADOR_IO_SERVICE_HPP

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
#define OOS_NET_API __declspec(dllexport)
#define EXPIMP_NET_TEMPLATE
#else
#define OOS_NET_API __declspec(dllimport)
#define EXPIMP_NET_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

#include "matador/utils/buffer.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"
#include "matador/net/stream_handler.hpp"

namespace matador {

/**
 * IO Service is used to encapsulate the an instance
 * of the reactor class.
 */
class OOS_NET_API io_service
{
public:
  /**
   * Creates a io_service
   */
  io_service();

  /**
   * Starts the io_service with the underlying reactor
   */
  void run();

  /**
   * Returns true if the io service is
   * running
   *
   * @return True if service is running
   */
  bool is_running() const;

  /**
   * Shuts down a running service
   */
  void shutdown();

  /**
   * Adds the given acceptor for the
   * given peer endpoint and callback.
   *
   * The callback is called, when a new connection
   * was accepted.
   *
   * @tparam AcceptCallback Type of callback
   * @param ac Acceptor used to accept connections
   * @param ep Endpoint on which the acceptor will listen
   * @param accept_callback Callback when connection was accepted
   */
  template < typename AcceptCallback >
  void accept(const std::shared_ptr<acceptor>& ac, const tcp::peer &ep, AcceptCallback accept_callback);

  /**
   * Adds the given acceptor for the
   * given callback.
   *
   * The callback is called, when a new connection
   * was accepted.
   *
   * @tparam AcceptCallback Type of callback
   * @param ac Acceptor used to accept connections
   * @param accept_callback Callback when connection was accepted
   */
  template < typename AcceptCallback >
  void accept(const std::shared_ptr<acceptor>& ac, AcceptCallback accept_callback);

  /**
   * Add the given connector for the given port
   * and execute callback.
   *
   * Once a connection is established the callback
   * is called.
   *
   * @tparam ConnectCallback Type of the callback
   * @param co Connector Used to establish the connection
   * @param port Port to execute to
   * @param connect_callback Callback when connection was established
   */
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
