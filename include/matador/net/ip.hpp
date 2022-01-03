#ifndef MATADOR_IP_HPP
#define MATADOR_IP_HPP

#include "matador/net/export.hpp"
#include "matador/net/peer.hpp"
#include "matador/net/address_resolver.hpp"
#include "matador/net/socket.hpp"
#include "matador/net/socket_stream.hpp"
#include "matador/net/socket_acceptor.hpp"

namespace matador {

/**
 * The tcp class represents all
 * settings to handle tcp socket
 * connections
 */
class OOS_NET_API tcp
{
public:
  typedef peer_base<tcp> peer;            /**< Shortcut to a tcp based peer */
  typedef socket_stream<tcp> socket;      /**< Shortcut to a tcp based socket */
  typedef socket_acceptor<tcp> acceptor;  /**< Shortcut to a tcp based acceptor */
  typedef address_resolver<tcp> resolver; /**< Shortcut to a tcp based address resolver */

  /**
   * Returns the type of the socket
   *
   * @return Type of the socket
   */
  int type() const { return SOCK_STREAM; }

  /**
   * Returns the socket protocol
   *
   * @return Socket protocol
   */
  int protocol() const { return IPPROTO_TCP; }

  /**
   * Returns the socket family
   *
   * @return Socket family
   */
  int family() const { return family_; }

  /**
   * Creates an instance of tcp representing
   * an IPv4 socket
   *
   * @return IPv4 tcp object
   */
  static tcp v4() { return tcp(PF_INET); }

  /**
   * Creates an instance of tcp representing
   * an IPv6 socket
   *
   * @return IPv6 tcp object
   */
  static tcp v6() { return tcp(PF_INET6); }

private:
  explicit tcp(int family)
  : family_(family)
  {}

  int family_;
};

/**
 * The udp class represents all
 * settings to handle udp socket
 * connections
 */
class OOS_NET_API udp
{
public:
  typedef peer_base<udp> peer;            /**< Shortcut to a udp based peer */
  typedef socket_stream<udp> socket;      /**< Shortcut to a udp based socket */
  typedef socket_acceptor<udp> acceptor;  /**< Shortcut to a udp based acceptor */
  typedef address_resolver<udp> resolver; /**< Shortcut to a udp based address resolver */

  /**
 * Returns the type of the socket
 *
 * @return Type of the socket
 */
  int type() const { return SOCK_DGRAM; }

  /**
   * Returns the socket protocol
   *
   * @return Socket protocol
   */
  int protocol() const { return IPPROTO_UDP; }

  /**
   * Returns the socket family
   *
   * @return Socket family
   */
  int family() const { return family_; }

  /**
   * Creates an instance of udp representing
   * an IPv4 socket
   *
   * @return IPv4 udp object
   */
  static udp v4() { return udp(PF_INET); }

  /**
   * Creates an instance of udp representing
   * an IPv6 socket
   *
   * @return IPv6 udp object
   */
  static udp v6() { return udp(PF_INET6); }

private:
  explicit udp(int family)
  : family_(family)
  {}

  int family_;
};


}

#endif //MATADOR_IP_HPP
