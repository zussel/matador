//
// Created by sascha on 1/19/16.
//

#ifndef OOS_HASMANYLISTUNITTEST_HPP
#define OOS_HASMANYLISTUNITTEST_HPP


#include "unit/unit_test.hpp"

#include "object/identifier.hpp"
#include "object/has_many.hpp"

namespace hasmanylist {

class item
{
public:
  oos::identifier<unsigned long> id;
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
  oos::identifier<unsigned long> id;
  std::string name;
  oos::has_many<item, std::list> items;

  owner() {}
  explicit owner(const std::string &n) : name(n) {}

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
    //s.serialize(items); // -> should create relation as in next line
    //s.serialize("owner_item",items); // -> should create relation as in next line
    s.serialize("owner_item", items, "owner_id", "item_id");
  }
};

class many_ints
{
public:
  typedef oos::has_many<int, std::list> int_list_t;

public:
  oos::identifier<unsigned long> id;
  int_list_t ints;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("ints", ints, "list_id", "value");
  }
};

}

class HasManyListUnitTest : public oos::unit_test
{

public:

  HasManyListUnitTest();
  virtual ~HasManyListUnitTest();

  virtual void initialize() override;
  virtual void finalize() override;

  void test_join_table();
  void test_const_iterator();
  void test_integer();
};


#endif //OOS_HASMANYLISTUNITTEST_HPP
