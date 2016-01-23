//
// Created by sascha on 1/19/16.
//

#include "HasManyListUnitTest.hpp"

using namespace oos;
using namespace hasmanylist;

HasManyListUnitTest::HasManyListUnitTest()
  : unit_test("list", "has many list unit tests")
{
  add_test("join", std::bind(&HasManyListUnitTest::test_join_table, this), "test list join table");
  add_test("const_iterator", std::bind(&HasManyListUnitTest::test_const_iterator, this), "test list const iterator");
  add_test("int", std::bind(&HasManyListUnitTest::test_integer, this), "test list of ints");
}

HasManyListUnitTest::~HasManyListUnitTest() { }

void HasManyListUnitTest::initialize() { }

void HasManyListUnitTest::finalize() { }

void HasManyListUnitTest::test_join_table()
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

void HasManyListUnitTest::test_const_iterator()
{
  many_ints mi;

  mi.ints.push_back(7);
  mi.ints.push_back(1);
  mi.ints.insert(mi.ints.begin(), 6);

  many_ints::int_list_t::const_iterator first = mi.ints.begin();
  many_ints::int_list_t::iterator i = mi.ints.end();
  many_ints::int_list_t::const_iterator last = i;

  
}

void HasManyListUnitTest::test_integer()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mi = store.insert(new many_ints);

  UNIT_ASSERT_EQUAL(mi->ints.size(), 0UL, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    mi->ints.push_back(i);
  }

  UNIT_ASSERT_EQUAL(mi->ints.size(), 20UL, "pointer vector has invalid size");

  many_ints::int_list_t::iterator i = mi->ints.begin();

  UNIT_ASSERT_EQUAL(*i, 0, "item is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, 1, "item is invalid");

  many_ints::int_list_t::item_ptr iptr = i.relation_item();

  UNIT_ASSERT_EQUAL(iptr->value(), 1, "item is invalid");

//  ival = 4;
//  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");
//
//  i = itemvector->erase(i);
//
////  cout << "\n";
//
////  std::for_each(itemvector->begin(), itemvector->end(), print);
//
//  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");
//  UNIT_ASSERT_EQUAL((int)itemvector->size(), 19, "itemvector size isn't valid");
}
