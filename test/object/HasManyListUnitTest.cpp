//
// Created by sascha on 1/19/16.
//

#include "HasManyListUnitTest.hpp"

#include "../has_many_list.hpp"
#include "../entities.hpp"

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
  UNIT_ASSERT_TRUE(i != store.end());

  UNIT_ASSERT_EQUAL("owner_item", i->type());

  object_ptr<owner> o = store.insert(new owner("hans"));

  UNIT_ASSERT_EQUAL("hans", o->name);

  object_ptr<item> i1 = store.insert(new item("i1"));

  UNIT_ASSERT_EQUAL("i1", i1->name);

  o.modify()->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size());

  o.modify()->items.push_back(new item("i2"));

  UNIT_ASSERT_EQUAL(2U, o->items.size());
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

  UNIT_ASSERT_FALSE(first == last);
  UNIT_ASSERT_TRUE(first == mi.elements.begin());
  UNIT_ASSERT_TRUE(last == mi.elements.end());

  ++first;

  UNIT_ASSERT_EQUAL(*first, 7);

  first++;

  UNIT_ASSERT_EQUAL(*first, 1);

  first--;

  UNIT_ASSERT_EQUAL(*first, 7);

  --first;

  UNIT_ASSERT_EQUAL(*first, 6);
}

void HasManyListUnitTest::test_erase_scalar()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mptr = store.insert(new many_ints);

  mptr.modify()->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size());

  mptr.modify()->elements.push_back(7);
  mptr.modify()->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size());

  auto i = mptr.modify()->elements.begin();

  i = mptr.modify()->elements.erase(i);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size());
  UNIT_ASSERT_EQUAL(*i, 7);

  mptr.modify()->elements.push_back(3);
  mptr.modify()->elements.push_back(4);

  i = mptr.modify()->elements.begin();

  auto j = i;
  ++j;
  ++j;

  i = mptr.modify()->elements.erase(i, j);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size());
  UNIT_ASSERT_EQUAL(*i, 3);
}

void HasManyListUnitTest::test_erase_object()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  object_ptr<owner> o = store.insert(new owner("hans"));
  object_ptr<item> i1 = store.insert(new item("i1"));

  UNIT_ASSERT_EQUAL("i1", i1->name);

  o.modify()->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size());

  o.modify()->items.push_back(new item("i2"));

  UNIT_ASSERT_EQUAL(2U, o->items.size());

  auto i = o.modify()->items.begin();

  i = o.modify()->items.erase(i);

  UNIT_ASSERT_EQUAL(1U, o->items.size());
  UNIT_ASSERT_EQUAL(i->name, "i2");

  o.modify()->items.push_back(new item("i3"));
  o.modify()->items.push_back(new item("i4"));

  i = o.modify()->items.begin();

  auto j = i;
  ++j;
  ++j;

  i = o.modify()->items.erase(i, j);

  UNIT_ASSERT_EQUAL(1U, o->items.size());
  UNIT_ASSERT_EQUAL(i->name, "i4");
}

void HasManyListUnitTest::test_remove_scalar()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  auto mptr = store.insert(new many_ints);

  mptr.modify()->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size());

  mptr.modify()->elements.push_back(7);
  mptr.modify()->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size());

  mptr.modify()->elements.remove(1);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size());

  mptr.modify()->elements.remove_if([](const int &val) {
    return val == 90;
  });

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size());
}

void HasManyListUnitTest::test_remove_object()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  object_ptr<owner> o = store.insert(new owner("hans"));
  object_ptr<item> i1 = store.insert(new item("i1"));
  object_ptr<item> i2 = store.insert(new item("i2"));

  UNIT_ASSERT_EQUAL("i1", i1->name);

  o.modify()->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size());

  o.modify()->items.push_back(i2);

  UNIT_ASSERT_EQUAL(2U, o->items.size());

  o.modify()->items.remove(i2);

  UNIT_ASSERT_EQUAL(1U, o->items.size());

  o.modify()->items.remove_if([&i1](const object_ptr<item> &x) {
    return i1 == x;
  });

  UNIT_ASSERT_EQUAL(0U, o->items.size());
  UNIT_ASSERT_TRUE(o->items.empty());
}

void HasManyListUnitTest::test_integer()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mi = store.insert(new many_ints);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL);

  for (int i = 0; i < 20; ++i) {
    mi.modify()->elements.push_back(i);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 20UL);

  auto i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, 0);

  i++;

  UNIT_ASSERT_EQUAL(*i, 1);

//  ival = 4;
//  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival);
//
//  i = itemvector->erase(i);
//
////  cout << "\n";
//
////  std::for_each(itemvector->begin(), itemvector->end(), print);
//
//  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival);
//  UNIT_ASSERT_EQUAL((int)itemvector->size(), 19);
}

void HasManyListUnitTest::test_string()
{
  object_store store;

  store.attach<many_strings>("many_strings");

  object_ptr<many_strings> mi = store.insert(new many_strings);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL);

  std::vector<std::string> names = { "george", "jane", "rudi", "hanna" };
  for (const auto &name : names) {
    mi.modify()->elements.push_back(name);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 4UL);

  auto i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, "george");

  i++;

  UNIT_ASSERT_EQUAL(*i, "jane");
}

//using many_list_varchars = many_builtins<matador::varchar<255>, std::list>;

void HasManyListUnitTest::test_varchar()
{
//  object_store store;
//
//  store.attach<many_list_varchars>("many_list_varchars");
//
//  object_ptr<many_list_varchars> mi = store.insert(new many_list_varchars);
//
//  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL);
//
//  std::vector<std::string> names = { "george", "jane", "rudi", "hanna" };
//  for (const auto &name : names) {
//    mi.modify()->elements.push_back(name.c_str());
//  }
//
//  UNIT_ASSERT_EQUAL(mi->elements.size(), 4UL);
//
//  auto i = mi->elements.begin();
//
//  UNIT_ASSERT_EQUAL(*i, "george");
//
//  i++;
//
//  UNIT_ASSERT_EQUAL(*i, "jane");
}
