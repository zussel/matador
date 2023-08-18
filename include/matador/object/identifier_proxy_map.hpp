#ifndef OOS_IDENTIFIER_PROXY_MAP_HPP
#define OOS_IDENTIFIER_PROXY_MAP_HPP

//#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/identifier.hpp"

#include "matador/object/basic_has_many_item_holder.hpp"

#include <unordered_map>

namespace matador {
class object_proxy;

namespace detail {

/// @cond MATADOR_DEV

//template<class T> class identifier_hash;

//template<>
//class identifier_hash<basic_identifier>
//{
//public:
//  size_t operator()(basic_identifier *id) const
//  {
//    return id->hash();
//  }
//};
//
//template<>
//class identifier_hash<std::shared_ptr<basic_identifier>>
//{
//public:
//  typedef std::shared_ptr<basic_identifier> value_type;
//
//  size_t operator()(const value_type &val) const
//  {
//    return val->hash();
//  }
//};
//
//class identifier_equal
//{
//public:
//  bool operator()(basic_identifier* lhs,
//                  basic_identifier* rhs) const
//  {
//    return lhs->is_same_type(*rhs) && lhs->equal_to(*rhs);
//  }
//};
//
//class identifier_equal_shared
//{
//public:
//  typedef std::shared_ptr<basic_identifier> value_type;
//
//  bool operator()(const value_type &lhs,
//                  const value_type &rhs) const
//  {
//    return lhs->is_same_type(*rhs) && lhs->equal_to(*rhs);
//  }
//};

using t_identifier_map = std::unordered_map<identifier, object_proxy*, id_pk_hash, std::equal_to<identifier>>;
typedef std::unordered_multimap<identifier, std::shared_ptr<basic_has_many_item_holder>, id_pk_hash, std::equal_to<identifier>> t_identifier_multimap;

/// @endcond

}
}

#endif //OOS_IDENTIFIER_PROXY_MAP_HPP
