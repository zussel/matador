#ifndef MATADOR_CONNECTOR_HPP
#define MATADOR_CONNECTOR_HPP

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

#include "matador/net/handler.hpp"
#include "matador/net/handler_creator.hpp"
#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

#include <functional>

namespace matador {

/**
 * Connector which initiates a reactor based connection
 * to a remote network host identified by a list
 * of endpoints.
 *
 * Once a connection is established a handler is created
 * with the given create handler function. The socket
 * is passed to the created handler.
 */
class OOS_NET_API connector : public handler, public handler_creator
{
public:
  typedef std::function<std::shared_ptr<handler>(tcp::socket sock, tcp::peer endpoint, connector *cnnctr)> t_connect_handler; /**< Shortcut to a function creating a handler on successfully execute to a host */

  /**
   * Default constructor
   */
  connector();

  /**
   * Creates a new connector with the given
   * create handler function
   *
   * @param on_new_connection Function which creates a handler on new connection
   */
  explicit connector(t_connect_handler on_new_connection);


  /**
   * Initiates a execute to one of the given endpoints within the
   * given reactor. Once a connection is established a new handler
   * for this connection is created. The new connection is dispatched
   * by the reactor.
   *
   * @param r Reactor to handle the connector and the created connection
   * @param endpoints List of endpoints to
   */
  void connect(reactor &r, const std::vector<tcp::peer> &endpoints);
  /**
   * Initiates a execute to one of the given endpoints within the
   * given reactor. Once a connection is established a new handler
   * for this connection is created with the given function. The new
   * connection is dispatched by the reactor.
   *
   * @param r Reactor to handle the connector and the created connection
   * @param endpoints List of endpoints to
   * @param on_new_connection Function creating a new handler on new connection
   */
  void connect(reactor &r, const std::vector<tcp::peer> &endpoints, t_connect_handler on_new_connection);

  /**
   * Opens the connector. Actually this does
   * nothing at all.
   */
  void open() override {}

  /**
   * Returns the handle of the connector. The result will
   * always be zero, because the connector doesn't has a socket
   *
   * @return Always zero
   */
  int handle() const override;

  /**
   * Does nothing
   */
  void on_input() override {}

  /**
   * Does nothing
   */
  void on_output() override {}

  /**
   * Does nothing
   */
  void on_except() override {}

  /**
   * The timeout call is used to establish the connection.
   * Once the connection could be established within this call
   * the timeout is canceled otherwise it will be tried again
   * after three seconds
   */
  void on_timeout() override;

  /**
   * Does nothing
   */
  void on_close() override {}

  /**
   * Does nothing
   */
  void close() override {}

  /**
   * Always false because connection is established via timeout
   *
   * @return Always false
   */
  bool is_ready_write() const override;

  /**
   * Always false because connection is established via timeout
   *
   * @return Always false
   */
  bool is_ready_read() const override;

  void notify_close(handler *hndlr) override;

  std::string name() const override;

private:
  t_connect_handler connect_handler_;

  logger log_;

  std::vector<tcp::peer> endpoints_;
};

}

#endif //MATADOR_CONNECTOR_HPP
