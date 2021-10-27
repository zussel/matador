#include "matador/net/address_resolver.hpp"

namespace matador {
namespace detail {

template<>
int determine_socktype<tcp>()
{
  return SOCK_STREAM;
}

template<>
int determine_socktype<udp>()
{
  return SOCK_DGRAM;
}

}
}