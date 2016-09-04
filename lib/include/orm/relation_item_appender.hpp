//
// Created by sascha on 8/26/16.
//

#ifndef OOS_RELATION_TABLE_LOADER_HPP
#define OOS_RELATION_TABLE_LOADER_HPP

#include "tools/access.hpp"
#include "tools/cascade_type.hpp"

#include <cstddef>
#include <iostream>

namespace oos {

namespace detail {

template < class OWNER >
class relation_item_appender
{
public:

  void append(const std::string &id, detail::t_identifier_map *identifier_proxy_map, basic_table::t_relation_item_map *has_many_relations)
  {
    has_many_relations_ = has_many_relations;
    identifier_proxy_map_ = identifier_proxy_map;
    owner_id_ = id;
    oos::access::serialize(*this, owner_);
    owner_id_.clear();
    identifier_proxy_map_ = nullptr;
    has_many_relations_ = nullptr;
  }

  template<class T>
  void serialize(T &x)
  {
    oos::access::serialize(*this, x);
  }

  template<class T>
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) { }

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) { }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &, const char *, const char *)
  {
    if (owner_id_ != id) {
      return;
    }
    std::cout << "loading relation table for '" << id << "'\n";
    for (auto owner : *identifier_proxy_map_) {
      std::cout << "appending items for owner " << *owner.first << "\n";
      auto rel = has_many_relations_->find(id);
      auto items = rel->second.equal_range(owner.first);
      for (auto item = items.first; item != items.second; ++item) {
        std::cout << "appending item " << *item->second->pk() << "\n";
      }
    }
  }

private:
  OWNER owner_;
  std::string owner_id_;
  detail::t_identifier_map *identifier_proxy_map_ = nullptr;
  basic_table::t_relation_item_map *has_many_relations_ = nullptr;
};

}
}
#endif //OOS_RELATION_TABLE_LOADER_HPP
