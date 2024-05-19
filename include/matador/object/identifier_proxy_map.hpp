#ifndef OOS_IDENTIFIER_PROXY_MAP_HPP
#define OOS_IDENTIFIER_PROXY_MAP_HPP

#include "matador/utils/identifier.hpp"

#include "matador/object/basic_container_item_holder.hpp"

#include <unordered_map>

namespace matador {
class object_proxy;

namespace detail {

/// @cond MATADOR_DEV

using t_identifier_map = std::unordered_map<identifier, object_proxy*, id_pk_hash, std::equal_to<identifier>>;
typedef std::unordered_multimap<identifier, std::shared_ptr<basic_container_item_holder>, id_pk_hash, std::equal_to<identifier>> t_identifier_multimap;

/// @endcond

}
}

#endif //OOS_IDENTIFIER_PROXY_MAP_HPP
