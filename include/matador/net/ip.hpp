#ifndef MATADOR_IP_HPP
#define MATADOR_IP_HPP

#include "matador/net/peer.hpp"
#include "matador/net/socket.hpp"

namespace matador {

class tcp
{
public:
  typedef peer_base<tcp> peer;
  typedef socket_stream<tcp> socket;
  typedef socket_acceptor<tcp> acceptor;
  typedef socket_connector<tcp> connector;

  int type() const { return SOCK_STREAM; }
  int protocol() const { return IPPROTO_TCP; }
  int family() const { return family_; }

  static tcp v4() { return tcp(PF_INET); }
  static tcp v6() { return tcp(PF_INET6); }

private:
  explicit tcp(int family)
  : family_(family)
  {}

  int family_;
};

class udp
{
public:
  typedef peer_base<udp> peer;
  typedef socket_stream<udp> socket;
  typedef socket_acceptor<udp> acceptor;
  typedef socket_connector<udp> connector;

  int type() const { return SOCK_DGRAM; }
  int protocol() const { return IPPROTO_UDP; }
  int family() const { return family_; }

  static udp v4() { return udp(PF_INET); }
  static udp v6() { return udp(PF_INET6); }

private:
  explicit udp(int family)
  : family_(family)
  {}

  int family_;
};

}

#endif //MATADOR_IP_HPP
