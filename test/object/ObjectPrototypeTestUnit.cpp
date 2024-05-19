#include "ObjectPrototypeTestUnit.hpp"

#include "matador/object/object_store.hpp"

#include "../datatypes.hpp"

#include <iostream>

using namespace matador;
using namespace std;

ObjectPrototypeTestUnit::ObjectPrototypeTestUnit()
  : unit_test("prototype", "ObjectStore Prototype Test Unit")
{
  add_test("empty", [this] { empty_store(); }, "test empty serializable store");
  add_test("find", [this] { test_find(); }, "find prototype test");
  add_test("size", [this] { test_size(); }, "size prototype test");
  add_test("parent_of", [this] { test_is_parent_of(); }, "check parent");
  add_test("decrement", [this] { test_decrement(); }, "check decrementing iterator");
  add_test("one", [this] { one_prototype(); }, "one prototype");
  add_test("hierarchy", [this] { prototype_hierarchy(); }, "prototype hierarchy");
  add_test("iterator", [this] { prototype_traverse(); }, "prototype iterator");
}

void
ObjectPrototypeTestUnit::empty_store()
{
  object_store store;

  UNIT_ASSERT_TRUE(store.empty());
}

void
ObjectPrototypeTestUnit::test_find()
{
  object_store store;
  store.attach<datatypes>("item");
  
  prototype_iterator i = store.find<datatypes>();
  
  UNIT_ASSERT_TRUE(i != store.end());
}

void
ObjectPrototypeTestUnit::test_size()
{
  object_store store;
  
  prototype_iterator i = store.begin();
  
  UNIT_ASSERT_TRUE(i->size() == 0);

  store.attach<datatypes>("item");
    
  i = store.find<datatypes>();

  UNIT_ASSERT_TRUE(i->size() == 0);
  
  store.insert(new datatypes);

  UNIT_ASSERT_TRUE(i->size() == 1);
}

void
ObjectPrototypeTestUnit::test_is_parent_of()
{
  object_store store;
  
  prototype_iterator root = store.begin();
  
  UNIT_ASSERT_TRUE(root->size() == 0);

  store.attach<datatypes>("item");
    
  prototype_iterator i = store.find<datatypes>();

  UNIT_ASSERT_FALSE(root->is_child_of(i.get()));

//  UNIT_ASSERT_TRUE(i->is_child_of(root.get()));
}

void
ObjectPrototypeTestUnit::test_decrement()
{
  object_store store;
  store.attach<datatypes>("item");
  store.attach<ItemA, datatypes>("item_a");

  prototype_iterator i = store.end();

  --i;

  UNIT_ASSERT_TRUE(--i == store.begin());
}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store store;

  store.attach<datatypes>("item");
  
  auto *o = store.create<datatypes>();
  
  UNIT_ASSERT_NOT_NULL(o);

  delete o;
  
  store.detach("item");
  
  UNIT_ASSERT_EXCEPTION(store.create<datatypes>(), object_exception, "unknown prototype type");
}

void
ObjectPrototypeTestUnit::prototype_hierarchy()
{
  object_store store;
  store.attach<datatypes>("ITEM");
  store.attach<ItemA, datatypes>("ITEM_A");
  store.attach<ItemB, datatypes>("ITEM_B");
  store.attach<ItemC, datatypes>("ITEM_C");

  auto *a = store.create<ItemB>();
  
  UNIT_ASSERT_NOT_NULL(a);
  
  delete a;
  
  store.detach("ITEM_B");
  
  UNIT_ASSERT_EXCEPTION(store.create<ItemB>(), object_exception, "unknown prototype type");
  
  store.detach("ITEM");
  
  UNIT_ASSERT_EXCEPTION(store.create<datatypes>(), object_exception, "unknown prototype type");
  UNIT_ASSERT_EXCEPTION(store.create<ItemA>(), object_exception, "unknown prototype type");
  UNIT_ASSERT_EXCEPTION(store.create<ItemC>(), object_exception, "unknown prototype type");
}

void
ObjectPrototypeTestUnit::prototype_traverse()
{
  object_store store;
  store.attach<datatypes>("ITEM");
  store.attach<ItemA, datatypes>("ITEM_A");
  store.attach<ItemB, datatypes>("ITEM_B");
  store.attach<ItemC, datatypes>("ITEM_C");

  prototype_iterator first = store.begin();
  prototype_iterator last = store.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 4);
    ++first;
    ++count;
  }
  
  UNIT_ASSERT_EQUAL(count, 4);
}
