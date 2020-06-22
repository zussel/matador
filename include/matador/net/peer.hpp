#ifndef MATADOR_PEER_HPP
#define MATADOR_PEER_HPP

#include "matador/net/address.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <cstring>

namespace matador {

template < class P >
class peer_base
{
public:
  typedef P protocol_type;
  typedef sockaddr_in address_type;

  explicit peer_base(const protocol_type &protocol, unsigned short port = 0)
  {
    memset(&sockaddr_, 0, sizeof(sockaddr_));
    sockaddr_.sin_family = protocol.family();
    sockaddr_.sin_port = htons(port);
    sockaddr_.sin_addr.s_addr = INADDR_ANY;
  }

  explicit peer_base(const address &addr, in_port_t port = 0)
  {
    memset(&sockaddr_, 0, sizeof(sockaddr_));
    sockaddr_.sin_family = PF_INET;
    sockaddr_.sin_addr.s_addr = addr.to_ulong();
    sockaddr_.sin_port = port;
  }

  peer_base(const peer_base &x)
  : sockaddr_(x.sockaddr_)
  {}

  ~peer_base() = default;

  address addr() const
  {
    return addr_;
  }

  int port() const { return sockaddr_.sin_port; }
  protocol_type protocol() const { return protocol_type::v4(); }

  sockaddr* data()
  {
    return (sockaddr *) &sockaddr_;
  }

  const sockaddr* data() const
  {
    return (const sockaddr*)(&sockaddr_);
  }

  size_t size() const
  {
    return sizeof(sockaddr_);
  }

private:
  sockaddr_in sockaddr_ = {};
  address addr_;
};

}
#endif //MATADOR_PEER_HPP
