#ifndef MATADOR_IP_HPP
#define MATADOR_IP_HPP

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

#include "matador/net/peer.hpp"
#include "matador/net/socket.hpp"

namespace matador {

class OOS_NET_API tcp
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

class OOS_NET_API udp
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
