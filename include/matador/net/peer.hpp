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

  peer_base() = default;

  explicit peer_base(address addr)
    : addr_(std::move(addr))
  {}

  peer_base(address addr, unsigned short port)
    : addr_(std::move(addr))
  {
    addr_.port(port);
  }

  peer_base(const peer_base &x)
    : addr_(x.addr_)
  {}

  peer_base(peer_base &&x) noexcept
    : addr_(std::move(x.addr_))
  {}

  peer_base& operator=(const peer_base &x)
  {
    addr_ = x.addr_;
    return *this;
  }

  peer_base& operator=(peer_base &&x) noexcept
  {
    addr_ = std::move(x.addr_);
    return *this;
  }

  ~peer_base() = default;

  int port() const { return addr_.port(); }

  protocol_type protocol() const
  {
    if (addr_.is_v4()) {
      return protocol_type::v4();
    } else {
      return protocol_type::v6();
    }
  }

  sockaddr* data()
  {
    return addr_.addr();
  }

  const sockaddr* data() const
  {
    return addr_.addr();
  }

  size_t size() const
  {
    return addr_.size();
  }

  address& addr()
  {
    return addr_;
  }

  const address& addr() const
  {
    return addr_;
  }

  std::string to_string() const
  {
    char addstr[INET6_ADDRSTRLEN + 8];
    const char *name;
    if (addr().is_v4()) {
      name = os::inet_ntop(addr_.addr()->sa_family, &addr_.addr_v4()->sin_addr, addstr, INET6_ADDRSTRLEN);
    } else {
      name = os::inet_ntop(addr_.addr()->sa_family, &addr_.addr_v6()->sin6_addr, addstr, INET6_ADDRSTRLEN);
    }

    size_t pos = strlen(name);

    snprintf(addstr+pos, INET6_ADDRSTRLEN+8-pos, ":%d", addr_.port());
    return addstr;
  }

private:
  address addr_;
};

}
#endif //MATADOR_PEER_HPP
