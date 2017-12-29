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

  UNIT_ASSERT_TRUE(ostore.empty(), "serializable store must be empty");
}

void
ObjectPrototypeTestUnit::test_find()
{
  object_store ostore;
  ostore.attach<Item>("item");
  
  prototype_iterator i = ostore.find<Item>();
  
  UNIT_ASSERT_TRUE(i != ostore.end(), "couldn't find prototype");
}

void
ObjectPrototypeTestUnit::test_size()
{
  object_store ostore;
  
  prototype_iterator i = ostore.begin();
  
  UNIT_ASSERT_TRUE(i->size() == 0, "prototype node must be empty");

  ostore.attach<Item>("item");
    
  i = ostore.find<Item>();

  UNIT_ASSERT_TRUE(i->size() == 0, "prototype node must be empty");
  
  ostore.insert(new Item);

  UNIT_ASSERT_TRUE(i->size() == 1, "prototype node must be one (1)");
}

void
ObjectPrototypeTestUnit::test_is_parent_of()
{
  object_store ostore;
  
  prototype_iterator root = ostore.begin();
  
  UNIT_ASSERT_TRUE(root->size() == 0, "prototype node must be empty");

  ostore.attach<Item>("item");
    
  prototype_iterator i = ostore.find<Item>();

  UNIT_ASSERT_FALSE(root->is_child_of(i.get()), "root must not be child of node");

//  UNIT_ASSERT_TRUE(i->is_child_of(root.get()), "node must be child of root");
}

void
ObjectPrototypeTestUnit::test_decrement()
{
  object_store ostore;
  ostore.attach<Item>("item");
  ostore.attach<ItemA, Item>("item_a", matador::object_store::not_abstract);

  prototype_iterator i = ostore.end();

  --i;

  UNIT_ASSERT_TRUE(--i == ostore.begin(), "iterator must be begin");
}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store ostore;

  ostore.attach<Item>("item");
  
  Item *o = ostore.create<Item>();
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create serializable of type <Item>");

  delete o;
  
  ostore.detach("item");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<Item>(), object_exception, "unknown prototype type", "create with invalid type");
}

void
ObjectPrototypeTestUnit::prototype_hierachy()
{
  object_store ostore;
  ostore.attach<Item>("ITEM");
  ostore.attach<ItemA, Item>("ITEM_A");
  ostore.attach<ItemB, Item>("ITEM_B");
  ostore.attach<ItemC, Item>("ITEM_C");

  ItemB *a = ostore.create<ItemB>();
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't create serializable of type <ItemB>");
  
  delete a;
  
  ostore.detach("ITEM_B");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemB>(), object_exception, "unknown prototype type", "create with invalid type");
  
  ostore.detach("ITEM");
  
  UNIT_ASSERT_EXCEPTION(ostore.create<Item>(), object_exception, "unknown prototype type", "create with invalid type");
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemA>(), object_exception, "unknown prototype type", "create with invalid type");
  UNIT_ASSERT_EXCEPTION(ostore.create<ItemC>(), object_exception, "unknown prototype type", "create with invalid type");
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
    UNIT_ASSERT_LESS(count, 4, "prototype count isn't valid");
    ++first;
    ++count;
  }
  
  UNIT_ASSERT_EQUAL(count, 4, "expected prototype size isn't 4");
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
