#include "matador/net/address.hpp"

#ifdef _WIN32
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdexcept>

namespace matador {

const char* address_router<V6>::IP6ADDR_MULTICAST_ALLNODES = "FF02::1";

address::address(const sockaddr_in &addr)
  : size_(sizeof(sockaddr_in))
{
  socket_address_.sa_in = addr;
}

address::address(const sockaddr_in6 &addr)
  : size_(sizeof(sockaddr_in6))
{
  socket_address_.sa_in6 = addr;
}

address& address::operator=(const address &x)
{
  if (this == &x) {
    return *this;
  }
  clear();
  size_ = x.size_;
  socket_address_ = x.socket_address_;
  return *this;
}

address::address(address &&x) noexcept
  : socket_address_(x.socket_address_)
  , size_(x.size_)
{
  x.size_ = 0;
}

address& address::operator=(address &&x) noexcept
{
  if (this == &x) {
    return *this;
  }
  clear();
  socket_address_ = x.socket_address_;
  size_ = x.size_;
  x.size_ = 0;
  return *this;
}

address::~address()
{
  clear();
}

unsigned int address::to_ulong() const
{
  if (socket_address_.sa_raw.sa_family == PF_INET) {
    return socket_address_.sa_in.sin_addr.s_addr;
  } else {
    return 0;
    //return reinterpret_cast<sockaddr_in6 *>(addr_)->sin6_addr;
  }
}

std::string address::to_string() const
{
  char addstr[INET6_ADDRSTRLEN];
  if (is_v4()) {
    os::inet_ntop(socket_address_.sa_raw.sa_family, &socket_address_.sa_in.sin_addr, addstr, INET6_ADDRSTRLEN);
  } else {
    os::inet_ntop(socket_address_.sa_raw.sa_family, &socket_address_.sa_in6.sin6_addr, addstr, INET6_ADDRSTRLEN);
  }
  return std::string(addstr);
}

void address::port(unsigned short pn)
{
  if (is_v4()) {
    socket_address_.sa_in.sin_port = htons(pn);
  } else {
    socket_address_.sa_in6.sin6_port = htons(pn);
  }
}

unsigned short address::port() const
{
  if (is_v4()) {
    return ntohs(socket_address_.sa_in.sin_port);
  } else {
    return ntohs(socket_address_.sa_in6.sin6_port);
  }
}

bool address::is_v4() const
{
  return socket_address_.sa_raw.sa_family == PF_INET;
}

bool address::is_v6() const
{
  return socket_address_.sa_raw.sa_family == PF_INET6;
}

sockaddr *address::addr()
{
  return &socket_address_.sa_raw;
}

const sockaddr *address::addr() const
{
  return &socket_address_.sa_raw;
}

sockaddr_in *address::addr_v4()
{
  return &socket_address_.sa_in;
}

const sockaddr_in *address::addr_v4() const
{
  return &socket_address_.sa_in;
}

sockaddr_in6 *address::addr_v6()
{
  return &socket_address_.sa_in6;
}

const sockaddr_in6 *address::addr_v6() const
{
  return &socket_address_.sa_in6;
}

socklen_t address::size() const
{
  return size_;
}

void address::clear()
{
  if (is_v4()) {
    memset(&socket_address_.sa_in, 0, sizeof(socket_address_.sa_in));
  } else {
    memset(&socket_address_.sa_in6, 0, sizeof(socket_address_.sa_in6));
  }
}
}
