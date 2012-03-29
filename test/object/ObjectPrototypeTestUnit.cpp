#include "ObjectPrototypeTestUnit.hpp"

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"
#include "object/prototype_node.hpp"

#include "Item.hpp"

using namespace oos;
using namespace std;

ObjectPrototypeTestUnit::ObjectPrototypeTestUnit()
  : unit_test("ObjectStore Prototype Test Unit")
{
  add_test("empty", std::tr1::bind(&ObjectPrototypeTestUnit::empty_store, this), "test empty object store");
  add_test("one", std::tr1::bind(&ObjectPrototypeTestUnit::one_prototype, this), "one prototype");
  add_test("hierarchy", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_hierachy, this), "prototype hierarchy");
  add_test("iterator", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_traverse, this), "prototype iterator");
}

ObjectPrototypeTestUnit::~ObjectPrototypeTestUnit()
{}

void
ObjectPrototypeTestUnit::initialize()
{}

void
ObjectPrototypeTestUnit::finalize()
{}

void
ObjectPrototypeTestUnit::empty_store()
{
  object_store ostore;
}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store ostore;
//  ostore.insert_prototype(new object_producer<Artist>, "ARTIST");
  ostore.insert_prototype<Item>("ITEM");
  
  object *o = ostore.create("ITEM");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <Item>");
  
  Item *i = dynamic_cast<Item*>(o);
  
  UNIT_ASSERT_NOT_NULL(i, "couldn't cast object to Item");
  
  delete i;
  
  ostore.remove_prototype("ITEM");
  
  o = ostore.create("ITEM");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
}

void
ObjectPrototypeTestUnit::prototype_hierachy()
{
  object_store ostore;
  ostore.insert_prototype<Item>("ITEM");
  ostore.insert_prototype<ItemA>("ITEM_A", false, "ITEM");
  ostore.insert_prototype<ItemB>("ITEM_B", false, "ITEM");
  ostore.insert_prototype<ItemC>("ITEM_C", false, "ITEM");

  object *o = ostore.create("ITEM_B");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ItemB>");
  
  ItemB *a = dynamic_cast<ItemB*>(o);
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to ItemB");
  
  delete a;
  
  ostore.remove_prototype("ITEM_B");
  
  o = ostore.create("ITEM_B");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
  
  ostore.remove_prototype("ITEM");
  
  o = ostore.create("ITEM_C");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
}

void
ObjectPrototypeTestUnit::prototype_traverse()
{
  object_store ostore;
  ostore.insert_prototype<Item>("ITEM");
  ostore.insert_prototype<ItemA>("ITEM_A", false, "ITEM");
  ostore.insert_prototype<ItemB>("ITEM_B", false, "ITEM");
  ostore.insert_prototype<ItemC>("ITEM_C", false, "ITEM");

  prototype_iterator first = ostore.begin();
  prototype_iterator last = ostore.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 5, "prototype count isn't valid");
    ++first;
    ++count;
  }
  
  UNIT_ASSERT_EQUAL(count, 5, "expected prototype size isn't 4");
}
