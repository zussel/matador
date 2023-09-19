#ifndef OOS_HAS_MANY_LIST_HPP
#define OOS_HAS_MANY_LIST_HPP

#include "matador/object/has_many.hpp"

namespace hasmanylist {

class item
{
public:
  unsigned long id{};
  std::string name;

  item() = default;
  explicit item(std::string n) : name(std::move(n)) {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name);
  }
};

class owner
{
public:
  typedef matador::has_many<item, std::list> item_list_t;
  unsigned long id{};
  std::string name;
  item_list_t items;

  owner() = default;
  explicit owner(std::string n) : name(std::move(n)) {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name);
    matador::access::has_many_(op, "owner_item", items, "owner_id", "item_id", matador::cascade_type::ALL);
  }
};
}

#endif //OOS_HAS_MANY_LIST_HPP
