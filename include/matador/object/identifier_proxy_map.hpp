#ifndef OOS_IDENTIFIER_PROXY_MAP_HPP
#define OOS_IDENTIFIER_PROXY_MAP_HPP

#include "identifier.hpp"

#include <unordered_map>

namespace matador {
class object_proxy;

namespace detail {

/// @cond MATADOR_DEV

using t_identifier_map = std::unordered_map<identifier, object_proxy*, id_pk_hash, std::equal_to<identifier>>;

/// @endcond

}
}

#endif //OOS_IDENTIFIER_PROXY_MAP_HPP
