#ifndef MATADOR_ADDRESS_HPP
#define MATADOR_ADDRESS_HPP

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

#include "matador/utils/os.hpp"
#include "matador/net/os.hpp"
#include "matador/net/error.hpp"

#include <string>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#else
#include <netinet/in.h>
#include <libnet.h>
#endif

namespace matador {

enum protocol_family {
  V4, V6
};

template < protocol_family PF >
class address_router;

class OOS_NET_API address
{
private:
  address() = default;

public:
  explicit address(const sockaddr_in &addr);
  explicit address(const sockaddr_in6 &addr);
  explicit address(sockaddr_in &&addr);
  explicit address(sockaddr_in6 &&addr);

  address(const address &x) = default;
  address& operator=(const address &x);

  address(address &&x) noexcept;
  address& operator=(address &&x) noexcept;
  ~address();

  unsigned int to_ulong() const;
  std::string to_string() const;

  void port(unsigned short pn);
  unsigned short port() const;

  bool is_v4() const;
  bool is_v6() const;

  sockaddr* addr();
  const sockaddr* addr() const;

  sockaddr_in* addr_v4();
  const sockaddr_in* addr_v4() const;

  sockaddr_in6* addr_v6();
  const sockaddr_in6* addr_v6() const;

  socklen_t size() const;

  typedef address_router<V4> v4;
  typedef address_router<V6> v6;

private:
  void clear();

private:
  template < protocol_family PF >
  friend class address_router;

  union socket_address {
    sockaddr sa_raw;
    sockaddr_in sa_in;
    sockaddr_in6 sa_in6;
  };

  socket_address socket_address_ {};

  socklen_t size_ = 0;
};

template <>
class address_router<V4>
{
public:
  address_router() = delete;
  address_router& operator=(const address_router&) = delete;
  address_router(const address_router&) = delete;
  address_router& operator=(address_router&&) = delete;
  address_router(address_router&&) = delete;

  static address empty() { return mk_address(INADDR_ANY); };
  static address any() { return mk_address(INADDR_ANY); }
  static address loopback() { return mk_address(INADDR_LOOPBACK); }
  static address broadcast() {return mk_address(INADDR_BROADCAST); }
  static address from_ip(const std::string &str) { return from_ip(str.c_str()); }
  static address from_ip(const char *str)
  {
    // now fill in the address info struct
    // create and fill the hints struct
    if (str == nullptr) {
      return address();
    }
    // get address from string

    sockaddr_in addr{};
    int ret = os::inet_pton(PF_INET, str, &(addr.sin_addr));
    if (ret == 1) {
      addr.sin_family = PF_INET;
      return address(addr);
    } else if (ret == 0) {
      detail::throw_logic_error("invalid ip address");
    } else {
      detail::throw_logic_error_with_errno("invalid ip address: %s", errno);
    }
    return address();
  }

  static address from_hostname(const std::string &str) { return from_hostname(str.c_str()); }
  static address from_hostname(const char *str)
  {
    // now fill in the address info struct
    // create and fill the hints struct
    if (str == nullptr) {
      return address();
    }
    // get address from string
    sockaddr_in addr{};

    int ret;

    if ((ret = os::inet_pton(AF_INET, str, &addr.sin_addr)) == 1) {
      addr.sin_family = PF_INET;
    } else if (ret == -1) {
      detail::throw_logic_error_with_errno("invalid address: %s", errno);
    } else { /* 0 == try name */
      struct addrinfo hints{};
      struct addrinfo *result = nullptr;

      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
      hints.ai_socktype = SOCK_STREAM; /* Stream socket */
      hints.ai_flags = AI_PASSIVE;    /* Numeric or net network hostname */
      hints.ai_protocol = 0;          /* Any protocol */
      hints.ai_canonname = nullptr;
      hints.ai_addr = nullptr;
      hints.ai_next = nullptr;

      int s = getaddrinfo(str, nullptr, &hints, &result);
      if (s != 0) {
        detail::throw_logic_error_with_gai_errno("invalid ip address (getaddrinfo): %s", s);
      }

      /* getaddrinfo() returns a list of address structures.
         Try each address until we successfully bind(2).
         If socket(2) (or bind(2)) fails, we (close the socket
         and) try the next address. */

      // take first address
      memcpy(&addr, result->ai_addr, sizeof(&result->ai_addr));

      freeaddrinfo(result);           /* No longer needed */
    }
    addr.sin_family = PF_INET;
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
    return address(addr);
  }

private:
  static address mk_address(unsigned int inaddr)
  {
    sockaddr_in addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = htonl(inaddr);
    return address(addr);
  }
};

template <>
class address_router<V6>
{
public:
  address_router() = delete;
  address_router& operator=(const address_router&) = delete;
  address_router(const address_router&) = delete;
  address_router& operator=(address_router&&) = delete;
  address_router(address_router&&) = delete;

  static address empty() { return mk_address(in6addr_any); }
  static address any() { return mk_address(in6addr_any); }
  static address loopback() { return mk_address(in6addr_loopback); }
  static address broadcast() {return mk_multicast_address(); }

private:
  static const char *IP6ADDR_MULTICAST_ALLNODES;

  static address mk_address(in6_addr in6addr)
  {
    sockaddr_in6 addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = PF_INET6;
    addr.sin6_addr = in6addr;
    return address(addr);
  }

  static address mk_multicast_address()
  {
    sockaddr_in6 addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = PF_INET6;
    inet_pton(AF_INET6, IP6ADDR_MULTICAST_ALLNODES, &addr.sin6_addr);
    return address(addr);
  }
//  address from_ip(const std::string &str);
//  address from_ip(const char *str);
//  address from_hostname(const std::string &str);
//  address from_hostname(const char *str);

};


}
#endif //MATADOR_ADDRESS_HPP
