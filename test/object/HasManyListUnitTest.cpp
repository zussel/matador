//
// Created by sascha on 1/19/16.
//

#include "HasManyListUnitTest.hpp"

#include "../has_many_list.hpp"
#include "../Item.hpp"

using namespace matador;
using namespace hasmanylist;

HasManyListUnitTest::HasManyListUnitTest()
  : unit_test("list", "has many list unit tests")
{
  add_test("join", std::bind(&HasManyListUnitTest::test_join_table, this), "test list join table");
  add_test("const_iterator", std::bind(&HasManyListUnitTest::test_const_iterator, this), "test list const iterator");
  add_test("remove_scalar", std::bind(&HasManyListUnitTest::test_remove_scalar, this), "test list remove scalar elements");
  add_test("remove_object", std::bind(&HasManyListUnitTest::test_remove_object, this), "test list remove object elements");
  add_test("erase_scalar", std::bind(&HasManyListUnitTest::test_erase_scalar, this), "test list erase scalar elements");
  add_test("erase_object", std::bind(&HasManyListUnitTest::test_erase_object, this), "test list erase object elements");
  add_test("int", std::bind(&HasManyListUnitTest::test_integer, this), "test list of ints");
  add_test("string", std::bind(&HasManyListUnitTest::test_string, this), "test list of strings");
  add_test("varchar", std::bind(&HasManyListUnitTest::test_varchar, this), "test list of varchars");
}

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

  mi.elements.push_back(7);
  mi.elements.push_back(1);
  mi.elements.insert(mi.elements.begin(), 6);

  many_ints::element_list_t::const_iterator first = mi.elements.begin();
  many_ints::element_list_t::iterator i = mi.elements.end();
  many_ints::element_list_t::const_iterator last = i;

  UNIT_ASSERT_FALSE(first == last, "first and last must not be equal");
  UNIT_ASSERT_TRUE(first == mi.elements.begin(), "first must be equal begin()");
  UNIT_ASSERT_TRUE(last == mi.elements.end(), "last must be equal end()");

  ++first;

  UNIT_ASSERT_EQUAL(*first, 7, "value of first must be '7'");

  first++;

  UNIT_ASSERT_EQUAL(*first, 1, "value of first must be '1'");

  first--;

  UNIT_ASSERT_EQUAL(*first, 7, "value of first must be '7'");

  --first;

  UNIT_ASSERT_EQUAL(*first, 6, "value of first must be '6'");
}

void HasManyListUnitTest::test_erase_scalar()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mptr = store.insert(new many_ints);

  mptr->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size(), "size should be 1 (one)");

  mptr->elements.push_back(7);
  mptr->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size(), "size should be 3 (three)");

  many_ints::element_list_t::iterator i = mptr->elements.begin();

  i = mptr->elements.erase(i);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size(), "size should be 2 (two)");
  UNIT_ASSERT_EQUAL(*i, 7, "name must be '7'");

  mptr->elements.push_back(3);
  mptr->elements.push_back(4);

  i = mptr->elements.begin();

  auto j = i;
  ++j;
  ++j;

  i = mptr->elements.erase(i, j);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size(), "size should be 2 (two)");
  UNIT_ASSERT_EQUAL(*i, 3, "name must be '3'");
}

void HasManyListUnitTest::test_erase_object()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  object_ptr<owner> o = store.insert(new owner("hans"));
  object_ptr<item> i1 = store.insert(new item("i1"));

  UNIT_ASSERT_EQUAL("i1", i1->name, "name should be 'i1'");

  o->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");

  o->items.push_back(new item("i2"));

  UNIT_ASSERT_EQUAL(2U, o->items.size(), "size should be 2 (two)");

  owner::item_list_t::iterator i = o->items.begin();

  i = o->items.erase(i);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");
  UNIT_ASSERT_EQUAL(i->name, "i2", "name must be 'i2");

  o->items.push_back(new item("i3"));
  o->items.push_back(new item("i4"));

  i = o->items.begin();

  auto j = i;
  ++j;
  ++j;

  i = o->items.erase(i, j);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");
  UNIT_ASSERT_EQUAL(i->name, "i4", "name must be 'i4'");
}

void HasManyListUnitTest::test_remove_scalar()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mptr = store.insert(new many_ints);

  mptr->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size(), "size should be 1 (one)");

  mptr->elements.push_back(7);
  mptr->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size(), "size should be 3 (three)");

  mptr->elements.remove(1);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size(), "size should be 2 (two)");

  mptr->elements.remove_if([](const int &val) {
    return val == 90;
  });

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size(), "size should be 1 (one)");
}

void HasManyListUnitTest::test_remove_object()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  object_ptr<owner> o = store.insert(new owner("hans"));
  object_ptr<item> i1 = store.insert(new item("i1"));
  object_ptr<item> i2 = store.insert(new item("i2"));

  UNIT_ASSERT_EQUAL("i1", i1->name, "name should be 'i1'");

  o->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");

  o->items.push_back(i2);

  UNIT_ASSERT_EQUAL(2U, o->items.size(), "size should be 2 (two)");

  o->items.remove(i2);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");

  o->items.remove_if([&i1](const object_ptr<item> &x) {
    return i1 == x;
  });

  UNIT_ASSERT_EQUAL(0U, o->items.size(), "size should be 0 (zero)");
  UNIT_ASSERT_TRUE(o->items.empty(), "list should be empty");
}

void HasManyListUnitTest::test_integer()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mi = store.insert(new many_ints);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL, "has many list is not empty");

  for (int i = 0; i < 20; ++i) {
    mi->elements.push_back(i);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 20UL, "has many list has invalid size");

  many_ints::element_list_t::iterator i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, 0, "item is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, 1, "item is invalid");

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

void HasManyListUnitTest::test_string()
{
  object_store store;

  store.attach<many_strings>("many_strings");

  object_ptr<many_strings> mi = store.insert(new many_strings);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL, "has many list is not empty");

  std::vector<std::string> names = { "george", "jane", "rudi", "hanna" };
  for (auto name : names) {
    mi->elements.push_back(name);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 4UL, "has many list has invalid size");

  many_strings::element_list_t::iterator i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, "george", "value is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, "jane", "value is invalid");
}

using many_list_varchars = many_builtins<matador::varchar<255>, std::list>;

void HasManyListUnitTest::test_varchar()
{
  object_store store;

  store.attach<many_list_varchars>("many_list_varchars");

  object_ptr<many_list_varchars> mi = store.insert(new many_list_varchars);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL, "has many list is not empty");

  std::vector<std::string> names = { "george", "jane", "rudi", "hanna" };
  for (auto name : names) {
    mi->elements.push_back(name.c_str());
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 4UL, "has many list has invalid size");

  auto i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, "george", "value is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, "jane", "value is invalid");  
}
