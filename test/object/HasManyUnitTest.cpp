//
// Created by sascha on 31.12.15.
//

#include "HasManyUnitTest.hpp"

#include "object/object_store.hpp"

using namespace oos;
using namespace hasmany;

HasManyUnitTest::HasManyUnitTest()
  : unit_test("has_many", "has many unit tests")
{
  add_test("vector_join", std::bind(&HasManyUnitTest::test_vector_join_table, this), "test vector join table");
}

HasManyUnitTest::~HasManyUnitTest() { }

void HasManyUnitTest::initialize() {}

void HasManyUnitTest::finalize() {}

void HasManyUnitTest::test_vector_join_table()
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
