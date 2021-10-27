#ifndef MATADOR_PEER_HPP
#define MATADOR_PEER_HPP

#include "matador/net/address.hpp"

#ifdef _WIN32
//#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

#include <cstring>

namespace matador {

/**
 * The peer_base class acts like the holder
 * of network endpoint information like socket
 * address and port. The template argument
 * sets the protocol type either TCP or UDP.
 *
 * @tparam P Protocol type
 */
template < class P >
class peer_base
{
public:
  typedef P protocol_type; /**< Short to protocol type */

  /**
   * Default constructor
   */
  peer_base() = default;

  /**
   * Creates a peer from a given address. Port is set
   * to zero.
   *
   * @param addr Address to create the peer from
   */
  explicit peer_base(address addr)
    : addr_(std::move(addr))
  {}

  /**
   * Creates a peer from a given address and port
   *
   * @param addr Address to create the peer from
   * @param port Port of the endpoint
   */
  peer_base(address addr, unsigned short port)
    : addr_(std::move(addr))
  {
    addr_.port(port);
  }

  /**
   * Copy creates a peer from the given peer
   *
   * @param x Peer to copy from
   */
  peer_base(const peer_base &x)
    : addr_(x.addr_)
  {}

  /**
   * Move creates a peer from a given peer
   *
   * @param x Peer to move from
   */
  peer_base(peer_base &&x) noexcept
    : addr_(std::move(x.addr_))
  {}

  /**
   * Copy assigns a given peer to this peer
   *
   * @param x Peer to assign
   * @return The assigned peer
   */
  peer_base& operator=(const peer_base &x)
  {
    addr_ = x.addr_;
    return *this;
  }

  /**
   * Assign moves the given peer to this peer
   *
   * @param x The peer to move assign
   * @return The moved peer
   */
  peer_base& operator=(peer_base &&x) noexcept
  {
    addr_ = std::move(x.addr_);
    return *this;
  }

  /**
   * Destructor
   */
  ~peer_base() = default;

  /**
   * Returns the current port of the peer.
   *
   * @return The current port
   */
  int port() const { return addr_.port(); }

  /**
   * Returns the current IP protocol of the peer
   * address which is either IPv4 or IPv6
   *
   * @return The current IP protocol
   */
  protocol_type protocol() const
  {
    if (addr_.is_v4()) {
      return protocol_type::v4();
    } else {
      return protocol_type::v6();
    }
  }

  /**
   * Returns the raw pointer to the sockaddr structure
   *
   * @return The raw pointer to the sockaddr structure
   */
  sockaddr* data()
  {
    return addr_.addr();
  }

  /**
   * Returns the raw pointer to the sockaddr structure
   *
   * @return The raw pointer to the sockaddr structure
   */
  const sockaddr* data() const
  {
    return addr_.addr();
  }

  /**
   * Returns the size of the underlying sockaddr structure
   *
   * @return The size of the underlying sockaddr structure
   */
  size_t size() const
  {
    return addr_.size();
  }

  /**
   * Returns a reference to the address
   *
   * @return A reference to the address
   */
  address& addr()
  {
    return addr_;
  }

  /**
   * Returns a reference to the address
   *
   * @return A reference to the address
   */
  const address& addr() const
  {
    return addr_;
  }

  /**
   * Converts the peer endpoint to a string in
   * the format [ip]:[port]
   *
   * @return Returns a string representation of the peer
   */
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
