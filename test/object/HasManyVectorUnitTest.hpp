//
// Created by sascha on 31.12.15.
//

#ifndef OOS_HASMANYUNITTEST_HPP
#define OOS_HASMANYUNITTEST_HPP


#include "matador/unit/unit_test.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/object/has_many.hpp"

namespace hasmanyvector {

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
  typedef matador::has_many<item> item_vector_t;
  matador::identifier<unsigned long> id;
  std::string name;
  item_vector_t items;

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

class many_ints
{
public:
  typedef matador::has_many<int> int_vector_t;
public:
  matador::identifier<unsigned long> id;
  int_vector_t ints;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("ints", ints, "list_id", "value");
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
  void test_integer();
};

#endif //OOS_HASMANYUNITTEST_HPP
