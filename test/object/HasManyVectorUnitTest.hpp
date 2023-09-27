#ifndef OOS_HASMANYUNITTEST_HPP
#define OOS_HASMANYUNITTEST_HPP


#include <utility>

#include "matador/unit/unit_test.hpp"

#include "matador/object/container.hpp"
#include "../datatypes.hpp"
#include "../entities.hpp"

namespace hasmanyvector {

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
  typedef matador::container<item> item_vector_t;
  unsigned long id{};
  std::string name;
  item_vector_t items;

  owner() = default;
  explicit owner(std::string n) : name(std::move(n)) {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name);
    matador::access::has_many(op, "owner_item", items, "owner_id", "item_id", matador::cascade_type::ALL);
  }
};

}

class HasManyVectorUnitTest : public matador::unit_test
{
public:
  HasManyVectorUnitTest();

  void test_join_table();
  void test_const_iterator();
  void test_erase_scalar();
  void test_erase_object();
  void test_remove_scalar();
  void test_remove_object();
  void test_remove_if();
  void test_integer();
  void test_string();
  void test_varchar();
};

#endif //OOS_HASMANYUNITTEST_HPP
