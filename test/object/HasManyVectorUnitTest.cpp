//
// Created by sascha on 31.12.15.
//

#include "HasManyVectorUnitTest.hpp"

#include "matador/object/object_store.hpp"

using namespace matador;
using namespace hasmanyvector;

HasManyVectorUnitTest::HasManyVectorUnitTest()
  : unit_test("vector", "has many vector unit tests")
{
  add_test("join", std::bind(&HasManyVectorUnitTest::test_join_table, this), "test vector join table");
  add_test("const_iterator", std::bind(&HasManyVectorUnitTest::test_const_iterator, this), "test vector const iterator");
  add_test("erase_scalar", std::bind(&HasManyVectorUnitTest::test_erase_scalar, this), "test vector erase scalar elements");
  add_test("erase_object", std::bind(&HasManyVectorUnitTest::test_erase_object, this), "test vector erase object elements");
  add_test("remove_scalar", std::bind(&HasManyVectorUnitTest::test_remove_scalar, this), "test vector remove scalar elements");
  add_test("remove_object", std::bind(&HasManyVectorUnitTest::test_remove_object, this), "test vector remove object elements");
  add_test("remove_if", std::bind(&HasManyVectorUnitTest::test_remove_if, this), "test vector remove if");
  add_test("int", std::bind(&HasManyVectorUnitTest::test_integer, this), "test vector of elements");
  add_test("string", std::bind(&HasManyVectorUnitTest::test_string, this), "test list of strings");
  add_test("varchar", std::bind(&HasManyVectorUnitTest::test_varchar, this), "test list of varchars");
}

void HasManyVectorUnitTest::test_join_table()
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

void HasManyVectorUnitTest::test_const_iterator()
{
  many_vector_ints mi;

  mi.elements.push_back(7);
  mi.elements.push_back(1);
  mi.elements.insert(mi.elements.begin(), 6);

  many_vector_ints::element_list_t::const_iterator first = mi.elements.begin();
  many_vector_ints::element_list_t::iterator i = mi.elements.end();
  many_vector_ints::element_list_t::const_iterator last = i;

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

void HasManyVectorUnitTest::test_erase_scalar()
{
  object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  object_ptr<many_vector_ints> mptr = store.insert(new many_vector_ints);

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

void HasManyVectorUnitTest::test_erase_object()
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

  i = o.modify()->items.erase(i, i+2);

  UNIT_ASSERT_EQUAL(1U, o->items.size());
  UNIT_ASSERT_EQUAL(i->name, "i4");
}

void HasManyVectorUnitTest::test_remove_scalar()
{
  object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  object_ptr<many_vector_ints> mptr = store.insert(new many_vector_ints);

  mptr.modify()->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size());

  mptr.modify()->elements.push_back(7);
  mptr.modify()->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size());

  mptr.modify()->elements.remove(7);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size());
}

void HasManyVectorUnitTest::test_remove_object()
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

  o.modify()->items.remove(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size());
}

void HasManyVectorUnitTest::test_remove_if()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  object_ptr<owner> o = store.insert(new owner("hans"));
  object_ptr<item> i1 = store.insert(new item("i1"));
  object_ptr<item> i2 = store.insert(new item("i2"));
  object_ptr<item> i3 = store.insert(new item("i3"));
  object_ptr<item> i4 = store.insert(new item("i4"));
  object_ptr<item> i5 = store.insert(new item("i5"));

  UNIT_ASSERT_EQUAL("i1", i1->name);

  o.modify()->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size());

  o.modify()->items.push_back(i2);

  UNIT_ASSERT_EQUAL(2U, o->items.size());

  o.modify()->items.push_back(i3);

  UNIT_ASSERT_EQUAL(3U, o->items.size());

  o.modify()->items.push_back(i4);
  o.modify()->items.push_back(i5);

  o.modify()->items.remove_if([&](const owner::item_vector_t::value_type &val) {
    return i1 == val || i3 == val;
  });

  UNIT_ASSERT_EQUAL(3U, o->items.size());
}

void HasManyVectorUnitTest::test_integer()
{
  object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  object_ptr<many_vector_ints> mi = store.insert(new many_vector_ints);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL);

  for (int i = 0; i < 20; ++i) {
    mi.modify()->elements.push_back(i);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 20UL);

  auto i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, 0);

  i++;

  UNIT_ASSERT_EQUAL(*i, 1);
}

void HasManyVectorUnitTest::test_string()
{
  object_store store;

  store.attach<many_vector_strings>("many_strings");

  object_ptr<many_vector_strings> mi = store.insert(new many_vector_strings);

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

using many_vector_varchars = many_builtins<varchar<255>, std::vector>;

void HasManyVectorUnitTest::test_varchar()
{
  object_store store;

  store.attach<many_vector_varchars>("many_list_varchars");

  object_ptr<many_vector_varchars> mi = store.insert(new many_vector_varchars);

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
