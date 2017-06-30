//
// Created by sascha on 31.12.15.
//

#include "HasManyVectorUnitTest.hpp"

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
  add_test("int", std::bind(&HasManyVectorUnitTest::test_integer, this), "test vector of elements");
  add_test("string", std::bind(&HasManyVectorUnitTest::test_string, this), "test list of strings");
}

void HasManyVectorUnitTest::test_join_table()
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

void HasManyVectorUnitTest::test_const_iterator()
{
  many_vector_ints mi;

  mi.elements.push_back(7);
  mi.elements.push_back(1);
  mi.elements.insert(mi.elements.begin(), 6);

  many_vector_ints::element_list_t::const_iterator first = mi.elements.begin();
  many_vector_ints::element_list_t::iterator i = mi.elements.end();
  many_vector_ints::element_list_t::const_iterator last = i;

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

void HasManyVectorUnitTest::test_erase_scalar()
{
  object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  object_ptr<many_vector_ints> mptr = store.insert(new many_vector_ints);

  mptr->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size(), "size should be 1 (one)");

  mptr->elements.push_back(7);
  mptr->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size(), "size should be 3 (three)");

  many_vector_ints::element_list_t::iterator i = mptr->elements.begin();

  i = mptr->elements.erase(i);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size(), "size should be 2 (two)");
  UNIT_ASSERT_EQUAL(*i, 7, "name must be '7'");

  mptr->elements.push_back(3);
  mptr->elements.push_back(4);

  i = mptr->elements.begin();

  i = mptr->elements.erase(i, i+2);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size(), "size should be 2 (two)");
  UNIT_ASSERT_EQUAL(*i, 3, "name must be '3'");
}

void HasManyVectorUnitTest::test_erase_object()
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

  owner::item_vector_t::iterator i = o->items.begin();

  i = o->items.erase(i);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");
  UNIT_ASSERT_EQUAL(i->name, "i2", "name must be 'i2");

  o->items.push_back(new item("i3"));
  o->items.push_back(new item("i4"));

  i = o->items.begin();

  i = o->items.erase(i, i+2);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");
  UNIT_ASSERT_EQUAL(i->name, "i4", "name must be 'i4'");
}

void HasManyVectorUnitTest::test_remove_scalar()
{
  object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  object_ptr<many_vector_ints> mptr = store.insert(new many_vector_ints);

  mptr->elements.push_back(1);

  UNIT_ASSERT_EQUAL(1U, mptr->elements.size(), "size should be 1 (one)");

  mptr->elements.push_back(7);
  mptr->elements.push_back(90);

  UNIT_ASSERT_EQUAL(3U, mptr->elements.size(), "size should be 3 (three)");

  mptr->elements.remove(7);

  UNIT_ASSERT_EQUAL(2U, mptr->elements.size(), "size should be 2 (two)");
}

void HasManyVectorUnitTest::test_remove_object()
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

  o->items.remove(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");
}

void HasManyVectorUnitTest::test_integer()
{
  object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  object_ptr<many_vector_ints> mi = store.insert(new many_vector_ints);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    mi->elements.push_back(i);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 20UL, "pointer vector has invalid size");

  many_vector_ints::element_list_t::iterator i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, 0, "item is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, 1, "item is invalid");
}

void HasManyVectorUnitTest::test_string()
{
  object_store store;

  store.attach<many_vector_strings>("many_strings");

  object_ptr<many_vector_strings> mi = store.insert(new many_vector_strings);

  UNIT_ASSERT_EQUAL(mi->elements.size(), 0UL, "has many list is not empty");

  std::vector<std::string> names = { "george", "jane", "rudi", "hanna" };
  for (auto name : names) {
    mi->elements.push_back(name);
  }

  UNIT_ASSERT_EQUAL(mi->elements.size(), 4UL, "has many list has invalid size");

  many_vector_strings::element_list_t::iterator i = mi->elements.begin();

  UNIT_ASSERT_EQUAL(*i, "george", "value is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, "jane", "value is invalid");
}
