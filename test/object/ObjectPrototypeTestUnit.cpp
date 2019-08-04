#include "ObjectPrototypeTestUnit.hpp"

#include "matador/object/object_store.hpp"

#include "../Item.hpp"

#include <iostream>

using namespace matador;
using namespace std;

ObjectPrototypeTestUnit::ObjectPrototypeTestUnit()
  : unit_test("prototype", "ObjectStore Prototype Test Unit")
{
  add_test("empty", std::bind(&ObjectPrototypeTestUnit::empty_store, this), "test empty serializable store");
  add_test("find", std::bind(&ObjectPrototypeTestUnit::test_find, this), "find prototype test");
  add_test("size", std::bind(&ObjectPrototypeTestUnit::test_size, this), "size prototype test");
  add_test("parent_of", std::bind(&ObjectPrototypeTestUnit::test_is_parent_of, this), "check parent");
  add_test("decrement", std::bind(&ObjectPrototypeTestUnit::test_decrement, this), "check decrementing iterator");
  add_test("one", std::bind(&ObjectPrototypeTestUnit::one_prototype, this), "one prototype");
  add_test("hierarchy", std::bind(&ObjectPrototypeTestUnit::prototype_hierachy, this), "prototype hierarchy");
  add_test("iterator", std::bind(&ObjectPrototypeTestUnit::prototype_traverse, this), "prototype iterator");
  add_test("relation", std::bind(&ObjectPrototypeTestUnit::prototype_relation, this), "prototype relation");
}

void
ObjectPrototypeTestUnit::empty_store()
{
  object_store ostore;

  UNIT_ASSERT_TRUE(ostore.empty());
}

void
ObjectPrototypeTestUnit::test_find()
{
  object_store ostore;
  ostore.attach<Item>("item");
  
  prototype_iterator i = ostore.find<Item>();
  
  UNIT_ASSERT_TRUE(i != ostore.end());
}

void
ObjectPrototypeTestUnit::test_size()
{
  object_store ostore;
  
  prototype_iterator i = ostore.begin();
  
  UNIT_ASSERT_TRUE(i->size() == 0);

  ostore.attach<Item>("item");
    
  i = ostore.find<Item>();

  UNIT_ASSERT_TRUE(i->size() == 0);
  
  ostore.insert(new Item);

  UNIT_ASSERT_TRUE(i->size() == 1);
}

void
ObjectPrototypeTestUnit::test_is_parent_of()
{
  object_store ostore;
  
  prototype_iterator root = ostore.begin();
  
  UNIT_ASSERT_TRUE(root->size() == 0);

  ostore.attach<Item>("item");
    
  prototype_iterator i = ostore.find<Item>();

  UNIT_ASSERT_FALSE(root->is_child_of(i.get()));

//  UNIT_ASSERT_TRUE(i->is_child_of(root.get()));
}

void
ObjectPrototypeTestUnit::test_decrement()
{
  object_store ostore;
  ostore.attach<Item>("item");
  ostore.attach<ItemA, Item>("item_a");

  prototype_iterator i = ostore.end();

  --i;

  UNIT_ASSERT_TRUE(--i == ostore.begin());
}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store ostore;

  ostore.attach<Item>("item");
  
  auto *o = ostore.create<Item>();
  
  UNIT_ASSERT_NOT_NULL(o);

  delete o;
  
  ostore.detach("item");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<Item>(), object_exception, "unknown prototype type");
}

void
ObjectPrototypeTestUnit::prototype_hierachy()
{
  object_store ostore;
  ostore.attach<Item>("ITEM");
  ostore.attach<ItemA, Item>("ITEM_A");
  ostore.attach<ItemB, Item>("ITEM_B");
  ostore.attach<ItemC, Item>("ITEM_C");

  auto *a = ostore.create<ItemB>();
  
  UNIT_ASSERT_NOT_NULL(a);
  
  delete a;
  
  ostore.detach("ITEM_B");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemB>(), object_exception, "unknown prototype type");
  
  ostore.detach("ITEM");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<Item>(), object_exception, "unknown prototype type");
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemA>(), object_exception, "unknown prototype type");
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemC>(), object_exception, "unknown prototype type");
}

void
ObjectPrototypeTestUnit::prototype_traverse()
{
  object_store ostore;
  ostore.attach<Item>("ITEM");
  ostore.attach<ItemA, Item>("ITEM_A");
  ostore.attach<ItemB, Item>("ITEM_B");
  ostore.attach<ItemC, Item>("ITEM_C");

  prototype_iterator first = ostore.begin();
  prototype_iterator last = ostore.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4);
    ++first;
    ++count;
  }
  
  UNIT_ASSERT_EQUAL(count, 4);
}

void
ObjectPrototypeTestUnit::prototype_relation()
{
  // Todo: complete test!
  object_store ostore;

//  ostore.attach<album>("album");
//  ostore.attach<track>("track");
//  ostore.attach<playlist>("playlist");
}
