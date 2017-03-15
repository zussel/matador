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

  item() {}
  explicit item(const std::string &n) : name(n) {}

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

  owner() {}
  explicit owner(const std::string &n) : name(n) {}

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
    s.serialize("owner_item", items, "owner_id", "item_id");
  }
};

//class many_ints
//{
//public:
//  typedef matador::has_many<int, std::list> int_list_t;
//
//public:
//  matador::identifier<unsigned long> id;
//  int_list_t ints;
//
//  template < class S >
//  void serialize(S &s)
//  {
//    s.serialize("id", id);
//    s.serialize("ints", ints, "list_id", "value");
//  }
//};

}

#endif //OOS_HAS_MANY_LIST_HPP
