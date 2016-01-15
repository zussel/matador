//
// Created by sascha on 31.12.15.
//

#include "HasManyVectorUnitTest.hpp"

#include "object/object_store.hpp"

using namespace oos;
using namespace hasmany;

HasManyVectorUnitTest::HasManyVectorUnitTest()
  : unit_test("vector", "has many unit tests")
{
  add_test("join", std::bind(&HasManyVectorUnitTest::test_vector_join_table, this), "test vector join table");
  add_test("int", std::bind(&HasManyVectorUnitTest::test_vector_integer, this), "test vector of ints");
}

HasManyVectorUnitTest::~HasManyVectorUnitTest() { }

void HasManyVectorUnitTest::initialize() {}

void HasManyVectorUnitTest::finalize() {}

void HasManyVectorUnitTest::test_vector_join_table()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  prototype_iterator i = store.find("owner_item");
  UNIT_ASSERT_TRUE(i != store.end(), "iterator shouldn't be end");

  UNIT_ASSERT_EQUAL("owner_item", i->type(), "type should be 'owner_item'");

  object_ptr<owner> o = store.insert(new owner("hans"));

  UNIT_ASSERT_EQUAL("hans", o->name, "name should be 'hans'");

  object_ptr<item> i1 = store.insert(new item("i1"));

  UNIT_ASSERT_EQUAL("i1", i1->name, "name should be 'i1'");

  o->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");

  o->items.push_back(new item("i2"));

  UNIT_ASSERT_EQUAL(2U, o->items.size(), "size should be 2 (two)");
}

class many_ints
{
public:
  oos::identifier<unsigned long> id;
  oos::has_many<int> ints;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("ints", ints, "list_id", "value");
  }
};
void HasManyVectorUnitTest::test_vector_integer()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mi = store.insert(new many_ints);

  mi->ints.push_back(7);
}
