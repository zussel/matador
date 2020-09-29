#ifndef MATADOR_ADDRESS_RESOLVER_HPP
#define MATADOR_ADDRESS_RESOLVER_HPP

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
#include "matador/net/error.hpp"

#include <vector>

namespace matador {
/// @cond MATADOR_DEV
class tcp;
class udp;

namespace detail {

template < class P >
int determine_socktype();

template <>
OOS_NET_API int determine_socktype<tcp>();

template <>
OOS_NET_API int determine_socktype<udp>();

template < class P >
void initialize_hints(struct addrinfo &hints, int flags) {
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = determine_socktype<P>();
  hints.ai_protocol = 0;
  hints.ai_flags = flags;
}
/// @endcond
}

/**
 * The address resolver resolves a given host and port
 * to a peer object representing the given address
 *
 * @tparam P Type of protocol
 */
template < class P >
class address_resolver
{
public:
  typedef typename P::peer peer; /**< Shortcut to peer type */

  /**
   * Default constructor
   */
  address_resolver() = default;

  /**
   * Resolves the given host and port to a list
   * of valid peers representing the ip addresses
   * of the host either in IPv4 or IPv6 format
   *
   * @param hostname Hostname to resolve
   * @param port Port to resolve
   * @return A list of peers representing the host and port
   */
  std::vector<peer> resolve(const std::string &hostname, const std::string &port);

  /**
   * Resolves the given host and port to a list
   * of valid peers representing the ip addresses
   * of the host either in IPv4 or IPv6 format
   *
   * @param hostname Hostname to resolve
   * @param port Port to resolve
   * @return A list of peers representing the host and port
   */
  std::vector<peer> resolve(const char *hostname, const char *port);

};

/// @cond MATADOR_DEV
template<class P>
std::vector<typename address_resolver<P>::peer> address_resolver<P>::resolve(const std::string &hostname, const std::string &port)
{
  return resolve(hostname.c_str(), port.c_str());
}

template < class P >
std::vector<typename address_resolver<P>::peer> address_resolver<P>::resolve(const char *hostname, const char *port)
{
  struct addrinfo hints = {};
  detail::initialize_hints<P>(hints, AI_PASSIVE);
  struct addrinfo* res = nullptr;
  struct addrinfo* head = nullptr;
  int err = getaddrinfo(hostname, port, &hints, &res);

  if (err != 0) {
    detail::throw_logic_error_with_gai_errno("error on getaddrinfo: %s", err);
  }

  head = res;
  std::vector<peer> peers;
  do {

    if (res->ai_family == PF_INET) {
      peers.push_back(peer(address(*(struct sockaddr_in*)res->ai_addr)));
    } else if (res->ai_family == PF_INET6) {
      peers.push_back(peer(address(*(struct sockaddr_in6*)res->ai_addr)));
    } // else -> not supported
  } while ( (res = res->ai_next) != nullptr);

  freeaddrinfo(head);

  return peers;
}
/// @endcond
}
#endif //MATADOR_ADDRESS_RESOLVER_HPP
