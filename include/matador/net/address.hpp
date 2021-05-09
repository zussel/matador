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
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

namespace matador {

/**
 * Enum representing the protocol
 * family IPv4 and IPv6
 */
enum protocol_family {
  V4, /**< IPv4 enum value */
  V6  /**< IPv6 enum value */
};

/// @cond MATADOR_DEV
template < protocol_family PF >
class address_router;
/// @endcond

/**
 * The address class represents a IPv4 or
 * IPv6 address.
 */
class OOS_NET_API address
{
public:
  /**
   * Default consstructor
   */
  address() = default;

  /**
   * Constructs an address from the given
   * addr representing a IPv4 socket address
   * structure
   *
   * @param addr Initial IPv4 Socket address
   */
  explicit address(const sockaddr_in &addr);

  /**
   * Constructs an address from the given
   * addr representing a IPv6 socket address
   * structure
   *
   * @param addr Initial IPv6 Socket address
   */
  explicit address(const sockaddr_in6 &addr);

  /**
   * Copy constructs an address from the
   * given address x
   *
   * @param x Address to copy from
   */
  address(const address &x) = default;

  /**
   * Copy assign an address from the
   * given address x
   *
   * @param x Address to assign from
   * @return The assigned address
   */
  address& operator=(const address &x);

  /**
   * Move copy constructs an address from the
   * given address x
   *
   * @param x Address to move copy from
   */
  address(address &&x) noexcept;

  /**
   * Move assign an address from the
   * given address x
   *
   * @param x Address to move assign from
   * @return The moved address
   */
  address& operator=(address &&x) noexcept;

  /**
   * Destructs the address
   */
  ~address();

  /**
   * Returns the address as unsigned long value
   *
   * @return The address as unsigned long value
   */
  unsigned int to_ulong() const;

  /**
   * Return the address in string format either
   * in IPv4 dotted format or in IPv6 colon based
   * format
   *
   * @return The address as string
   */
  std::string to_string() const;

  /**
   * Sets the port number
   *
   * @param pn Port number to set
   */
  void port(unsigned short pn);

  /**
   * Returns the current port number of the
   * address.
   *
   * @return The current port number
   */
  unsigned short port() const;

  /**
   * Returns true if address is IPv4
   *
   * @return True if IPv4 address
   */
  bool is_v4() const;

  /**
   * Returns true if address is IPv4
   *
   * @return True if IPv4 address
   */
  bool is_v6() const;

  /**
   * Returns the raw sockaddr structure
   *
   * @return The raw sockaddr structure
   */
  sockaddr* addr();

  /**
   * Returns the raw sockaddr structure
   *
   * @return The raw sockaddr structure
   */
  const sockaddr* addr() const;

  /**
   * Returns the IPv4 sockaddr_in structure
   *
   * @return The IPv4 sockaddr_in structure
   */
  sockaddr_in* addr_v4();

  /**
   * Returns the IPv4 sockaddr_in structure
   *
   * @return The IPv4 sockaddr_in structure
   */
  const sockaddr_in* addr_v4() const;

  /**
   * Returns the IPv6 sockaddr_in6 structure
   *
   * @return The IPv6 sockaddr_in6 structure
   */
  sockaddr_in6* addr_v6();

  /**
   * Returns the IPv6 sockaddr_in6 structure
   *
   * @return The IPv6 sockaddr_in6 structure
   */
  const sockaddr_in6* addr_v6() const;

  /**
   * Returns the size of the underlying
   * address structure.
   *
   * @return Size of the underlying address structure
   */
  socklen_t size() const;

  typedef address_router<V4> v4; /**< Shortcut to the internal IPv4 address router */
  typedef address_router<V6> v6; /**< Shortcut to the internal IPv6 address router */

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

/// @cond MATADOR_DEV
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

  static address from_ip(const std::string &str)
  {
    return from_ip(str.c_str());
  }

  static address from_ip(const char *str)
  {
    // now fill in the address info struct
    // create and fill the hints struct
    if (str == nullptr) {
      return address();
    }
    // get address from string

    sockaddr_in6 addr{};
    int ret = os::inet_pton(PF_INET6, str, &(addr.sin6_addr));
    if (ret == 1) {
      addr.sin6_family = PF_INET6;
      return address(addr);
    } else if (ret == 0) {
      char message_buffer[1024];
      os::sprintf(message_buffer, 1024, "INET_PTON (ip): invalid ip address [%s]", str);
      detail::throw_logic_error(message_buffer);
    } else {
      char message_buffer[1024];
      int err = errno;
      os::sprintf(message_buffer, 1024, "INET_PTON (ip): invalid ip address [%s]: %%s", str);
      detail::throw_logic_error_with_errno(message_buffer, err);
    }
    return address();
  }

  static address from_hostname(const std::string &str)
  {
    return from_hostname(str.c_str());
  }

  static address from_hostname(const char *str)
  {
    // now fill in the address info struct
    // create and fill the hints struct
    if (str == nullptr) {
      return address();
    }
    // get address from string
    sockaddr_in6 addr{};

    int ret = os::inet_pton(PF_INET6, str, &addr.sin6_addr);

    if (ret == 1) {
      addr.sin6_family = PF_INET6;
    } else if (ret == -1) {
      char message_buffer[1024];
      int err = errno;
      os::sprintf(message_buffer, 1024, "INET_PTON (host): invalid ip address [%s] (errno: %d): %%s", str, err);
      detail::throw_logic_error_with_errno(message_buffer, err);
    } else { /* 0 == try name */
      struct addrinfo hints{};
      struct addrinfo *result = nullptr;

      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 */
      hints.ai_socktype = SOCK_STREAM; /* Stream socket */
      hints.ai_flags = AI_PASSIVE;    /* Numeric or net network hostname */
      hints.ai_protocol = 0;          /* Any protocol */
      hints.ai_canonname = nullptr;
      hints.ai_addr = nullptr;
      hints.ai_next = nullptr;

      int s = getaddrinfo(str, nullptr, &hints, &result);
      if (s != 0) {
        char message_buffer[1024];
        os::sprintf(message_buffer, 1024, "GETADDRINFO (host): invalid ip address [%s] (errno: %d): %%s", str, s);
        detail::throw_logic_error_with_errno(message_buffer, s);
      }

      /* getaddrinfo() returns a list of address structures.
         Try each address until we successfully bind(2).
         If socket(2) (or bind(2)) fails, we (close the socket
         and) try the next address. */

      // take first address
      memcpy(&addr, result->ai_addr, result->ai_addrlen);

      freeaddrinfo(result);           /* No longer needed */
    }
    addr.sin6_family = PF_INET6;
    return address(addr);
  }

private:
  static OOS_NET_API const char *IP6ADDR_MULTICAST_ALLNODES;

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
    os::inet_pton(AF_INET6, IP6ADDR_MULTICAST_ALLNODES, &addr.sin6_addr);
    return address(addr);
  }
};

/// @endcond

}
#endif //MATADOR_ADDRESS_HPP
