#include "matador/net/address.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdexcept>

namespace matador {

const char* address_router<V6>::IP6ADDR_MULTICAST_ALLNODES = "FF02::1";

address::address()
{
//  memset(&addr_, 0, sizeof(addr_));
//  addr_.sin_family = PF_INET;
}

address::address(const sockaddr_in *addr)
  : addr_((struct sockaddr*)addr)
  , size_(sizeof(sockaddr_in))
{}

address::address(const sockaddr_in6 *addr)
  : addr_((struct sockaddr*)addr)
  , size_(sizeof(sockaddr_in6))
{}

unsigned int address::to_ulong() const
{
  if (addr_->sa_family == PF_INET) {
    return reinterpret_cast<const sockaddr_in *>(addr_)->sin_addr.s_addr;
  } else {
    return 0;
    //return reinterpret_cast<sockaddr_in6 *>(addr_)->sin6_addr;
  }
}

std::string address::to_string() const
{
  if (addr_->sa_family == PF_INET) {
    char addstr[INET_ADDRSTRLEN];
    auto addr6 = reinterpret_cast<const sockaddr_in *>(addr_);
    const char *str = inet_ntop(addr6->sin_family, &(addr6->sin_addr), addstr, INET_ADDRSTRLEN);
    if (str == nullptr) {
      throw std::logic_error("inet_ntop error");
    }
    return str;
  } else {
    return std::string();
  }

//  return inet_ntoa(addr_.sin_addr);
}

void address::port(unsigned short pn)
{
  if (addr_->sa_family == PF_INET) {
    reinterpret_cast<sockaddr_in*>(addr_)->sin_port = htons(pn);
  } else {
    reinterpret_cast<sockaddr_in6*>(addr_)->sin6_port = htons(pn);
  }
}

unsigned short address::port() const
{
  if (addr_->sa_family == PF_INET) {
    return reinterpret_cast<sockaddr_in*>(addr_)->sin_port;
  } else {
    return reinterpret_cast<sockaddr_in6*>(addr_)->sin6_port;
  }
}

bool address::is_v4() const
{
  return addr_->sa_family == PF_INET;
}

bool address::is_v6() const
{
  return addr_->sa_family == PF_INET6;
}

sockaddr *address::addr() const
{
  return addr_;
}

socklen_t address::size() const
{
  return size_;
}
}
