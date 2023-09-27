#ifndef OOS_RELATION_TABLE_LOADER_HPP
#define OOS_RELATION_TABLE_LOADER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"

#include <cstddef>
#include <iostream>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

template < class OWNER >
class relation_item_appender
{
public:

  void append(const std::string &id, detail::t_identifier_map &identifier_proxy_map, basic_table::t_relation_item_map *has_many_relations)
  {
    has_many_relations_ = has_many_relations;
    owner_id_ = id;
    for (const auto& value : identifier_proxy_map) {
      id_ = value.first;
      matador::access::process(*this, *value.second->obj<OWNER>());
    }
    owner_id_.clear();
    identifier_proxy_map.clear();
    has_many_relations_ = nullptr;
  }

  template<class T>
  void serialize(T &x)
  {
    matador::access::process(*this, x);
  }

  template < class T >
  void on_primary_key(const char *, T &, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class T>
  void on_attribute(const char *, T &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes)  {}

  void on_belongs_to(const char*, identifiable_holder&, cascade_type) { }
  void on_has_one(const char*, identifiable_holder&, cascade_type) { }

  template<class V, template<class ...> class C>
  void on_has_many(const char *id, basic_container<V, C> &x, const char *, const char *, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }

  template<class V, template<class ...> class C>
  void on_has_many(const char *id, basic_container<V, C> &, cascade_type)
  {
    if (owner_id_ != id) {
      return;
    }
    auto rel = has_many_relations_->find(id);
    if (rel == has_many_relations_->end()) {
      return;
    }
    auto items = rel->second.equal_range(id_);
    for (auto item = items.first; item != items.second; ++item) {
//      typename basic_has_many<V, C>::internal_type val(item->second);
//      x.append(val);
    }
    // clear all elements
    rel->second.erase(id_);
  }

private:
  std::string owner_id_;
  identifier id_;
  basic_table::t_relation_item_map *has_many_relations_ = nullptr;
};

/// @endcond

}
}
#endif //OOS_RELATION_TABLE_LOADER_HPP
