//
// Created by sascha on 31.12.15.
//

#ifndef OOS_HASMANYUNITTEST_HPP
#define OOS_HASMANYUNITTEST_HPP


#include "unit/unit_test.hpp"

#include "object/identifier.hpp"
#include "object/has_many.hpp"

namespace hasmany {

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
  oos::has_many<item> items;

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

}

class HasManyUnitTest : public oos::unit_test
{

public:
  HasManyUnitTest();
  virtual ~HasManyUnitTest();

  virtual void initialize() override;
  virtual void finalize() override;

  void test_vector_join_table();
};

#endif //OOS_HASMANYUNITTEST_HPP
