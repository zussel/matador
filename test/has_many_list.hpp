//
// Created by sascha on 9/7/16.
//

#ifndef OOS_HAS_MANY_LIST_HPP
#define OOS_HAS_MANY_LIST_HPP

#include "matador/utils/identifier.hpp"

#include "matador/object/has_many.hpp"

namespace hasmanylist {

class item
{
public:
  matador::identifier<unsigned long> id;
  std::string name;

  item() = default;
  explicit item(std::string n) : name(std::move(n)) {}

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
  }
};

class owner
{
public:
  typedef matador::has_many<item, std::list> item_list_t;
  matador::identifier<unsigned long> id;
  std::string name;
  item_list_t items;

  owner() = default;
  explicit owner(std::string n) : name(std::move(n)) {}

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
    s.serialize("owner_item", items, "owner_id", "item_id", matador::cascade_type::ALL);
  }
};
}

#endif //OOS_HAS_MANY_LIST_HPP
