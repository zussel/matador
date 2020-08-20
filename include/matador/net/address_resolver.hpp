#ifndef MATADOR_ADDRESS_RESOLVER_HPP
#define MATADOR_ADDRESS_RESOLVER_HPP

#include "matador/net/peer.hpp"
#include "matador/net/error.hpp"

#include <vector>

namespace matador {

namespace detail {
template < class P >
void initialize_hints(struct addrinfo &hints) {
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
}
}

template < class P >
class address_resolver
{
public:
  typedef typename P::peer peer;

  address_resolver() = default;

  std::vector<peer> resolve(const char *hostname, const char *port);

};

template < class P >
std::vector<typename address_resolver<P>::peer> address_resolver<P>::resolve(const char *hostname, const char *port)
{
  struct addrinfo hints = {};
  detail::initialize_hints<P>(hints);
  struct addrinfo* res = nullptr;
  struct addrinfo* head = nullptr;
  int err = getaddrinfo(hostname, port, &hints, &res);

  if (err != 0) {
    detail::throw_logic_error_with_gai_errno("error on getaddrinfo: %s", err);
  }

  std::vector<peer> peers;
  do {

    if (res->ai_family == PF_INET) {

    } else if (res->ai_family == PF_INET6) {

    } // else -> not supported
  } while ( (res = res->ai_next) != nullptr);
  return std::move(peers);
}

}
#endif //MATADOR_ADDRESS_RESOLVER_HPP
