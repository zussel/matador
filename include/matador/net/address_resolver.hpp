#ifndef MATADOR_ADDRESS_RESOLVER_HPP
#define MATADOR_ADDRESS_RESOLVER_HPP

#include "matador/net/peer.hpp"
#include "matador/net/error.hpp"

#include <vector>

namespace matador {

class tcp;
class udp;

namespace detail {

template < class P >
int determine_socktype();

template <>
int determine_socktype<tcp>();

template <>
int determine_socktype<udp>();

template < class P >
void initialize_hints(struct addrinfo &hints, int flags) {
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = determine_socktype<P>();
  hints.ai_protocol = 0;
  hints.ai_flags = flags;
}

}

template < class P >
class address_resolver
{
public:
  typedef typename P::peer peer;

  address_resolver() = default;

  std::vector<peer> resolve(const std::string &hostname, const std::string &port);
  std::vector<peer> resolve(const char *hostname, const char *port);

};

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

}
#endif //MATADOR_ADDRESS_RESOLVER_HPP
