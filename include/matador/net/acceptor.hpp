#ifndef MATADOR_ACCEPTOR_HPP
#define MATADOR_ACCEPTOR_HPP

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
 * The acceptor class is used to accept new connection
 * within the reactor dispatcher.
 *
 * Once a new connection was accepted by the acceptor a
 * new handler is created and registered within the reactor
 * to handle the established connection
 */
class OOS_NET_API acceptor : public handler, public handler_creator
{
public:
  typedef std::function<std::shared_ptr<handler>(tcp::socket sock, tcp::peer endpoint, acceptor *accptr)> t_accept_handler; /**< Shortcut to a function creating a handler on successfully accepted a new connection */

  /**
   * Default constructor
   */
  acceptor();

  /**
   * Creates an acceptor with the given endpoint. The endpoint
   * represents the address on which the acceptor listens for new
   * connections
   *
   * @param endpoint Endpoint to listen for new connections
   */
  explicit acceptor(tcp::peer endpoint);

  /**
   * Creates an acceptor with the given endpoint. The endpoint
   * represents the address on which the acceptor listens for new
   * connections. The given function is called when a new
   * connection was accepted and returns a new handler for
   * the new connection.
   *
   * @param endpoint Endpoint to listen for new connections
   * @param on_new_connection Function creating a new handler for each accepted new connection
   */
  acceptor(tcp::peer endpoint, t_accept_handler on_new_connection);

  /**
   * Destructor
   */
  ~acceptor() override;

  /**
   * When a new connection is accepted the given function
   * is called to create a new handler for the connection
   *
   * @param on_new_connection Function creating a new handler for the new connection
   */
  void accecpt(t_accept_handler on_new_connection);

  /**
   * Accepts new connection at the given endpoint.
   * When a new connection is accepted the given function
   * is called to create a new handler for the connection.
   *
   * @param endpoint Endpoint to listen for new connections
   * @param on_new_connection Function creating a new handler for the new connection
   */
  void accecpt(const tcp::peer& endpoint, t_accept_handler on_new_connection);

  /**
   * Opens the acceptor means the socket address of the
   * endpoint is bound to the created listing socket
   * Then the socket is used for listening for new
   * connections.
   */
  void open() override;

  /**
   * Returns the current listening socket fd
   *
   * @return Listening socket fd
   */
  int handle() const override;

  /**
   * Is called when a new connection wants to
   * execute to the endpoint. Once the connection was accepted
   * a new connection handler is created and the socket is
   * passed to the handler. The handler is then registered
   * to the reactor to disptach its read and write events.
   */
  void on_input() override;

  /**
   * Does actually nothing
   */
  void on_output() override {}

  /**
   * Does actually nothing
   */
  void on_except() override {}

  /**
   * Does actually nothing
   */
  void on_timeout() override {}

  /**
   * Does actually nothing
   */
  void on_close() override {}

  /**
   * Closes the listebn fd of the acceptor
   */
  void close() override;

  /**
   * Returns always false because new connections
   * are indicated as read events.
   *
   * @return Always false
   */
  bool is_ready_write() const override;

  /**
   * Returns true if the acceptor was opened
   * and a listen fd was created.
   *
   * @return True If a listen socket was created
   */
  bool is_ready_read() const override;

  void notify_close(handler *hndlr) override;

  std::string name() const override;

public:
  /**
   * Returns the current endpoint accepting new connection.
   *
   * @return Current listening endpoint
   */
  const tcp::peer& endpoint() const;

private:
  tcp::acceptor acceptor_;
  tcp::peer endpoint_;

  t_accept_handler accept_handler_;

  logger log_;

  tcp::peer create_client_endpoint() const;
};

}
#endif //MATADOR_ACCEPTOR_HPP
