/**
 * @author sascha on 5/2/16.
 */

#ifndef OOS_IDENTIFIER_PROXY_MAP_HPP
#define OOS_IDENTIFIER_PROXY_MAP_HPP

#include "tools/basic_identifier.hpp"

#include <unordered_map>

namespace oos {
class object_proxy;

namespace detail {

template<class T> class identifier_hash;

/// @cond OOS_DEV
template<>
class identifier_hash<std::shared_ptr<basic_identifier> >
{
public:
  size_t operator()(const std::shared_ptr<basic_identifier> &id) const
  {
    return id->hash();
  }
};

class identifier_equal
{
public:
  bool operator()(const std::shared_ptr<basic_identifier>& lhs,
                  const std::shared_ptr<basic_identifier>& rhs) const
  {
    return lhs->is_same_type(*rhs) && lhs->equal_to(*rhs);
  }
};

typedef std::shared_ptr<basic_identifier> identifier_ptr; /**< Shortcut to shared identifier ptr */
typedef std::unordered_map<identifier_ptr, object_proxy*, identifier_hash<identifier_ptr>, identifier_equal> t_identifier_map;

}
}

#endif //OOS_IDENTIFIER_PROXY_MAP_HPP
