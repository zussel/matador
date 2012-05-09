#include "ObjectVectorTestUnit.hpp"

#include "Item.hpp"

#include "object/object_vector.hpp"
#include "object/object_view.hpp"

#include <iostream>

using namespace oos;
using namespace std;

ObjectVectorTestUnit::ObjectVectorTestUnit()
  : unit_test("object vector")
{
  add_test("int", std::tr1::bind(&ObjectVectorTestUnit::test_int_vector, this), "test object vector with integers");
  add_test("ptr", std::tr1::bind(&ObjectVectorTestUnit::test_ptr_vector, this), "test object vector with pointers");
  add_test("ref", std::tr1::bind(&ObjectVectorTestUnit::test_ref_vector, this), "test object vector with references");
}

ObjectVectorTestUnit::~ObjectVectorTestUnit()
{}

void ObjectVectorTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("ITEM");
  ostore_.insert_prototype<ItemPtrVector>("ITEM_PTR_VECTOR");
  ostore_.insert_prototype<ItemPtrVector::item_type>("ITEM_PTR");
  ostore_.insert_prototype<ItemRefVector>("ITEM_REF_VECTOR");
  ostore_.insert_prototype<ItemRefVector::item_type>("ITEM_REF");
  ostore_.insert_prototype<IntVector>("ITEM_INT_VECTOR");
  ostore_.insert_prototype<IntVector::item_type>("ITEM_INT");
}

void ObjectVectorTestUnit::finalize()
{
}

void ObjectVectorTestUnit::test_ref_vector()
{
  typedef object_ptr<ItemRefVector> itemvector_ptr;
  typedef ItemRefVector::value_type item_ptr;

  itemvector_ptr itemvector = ostore_.insert(new ItemRefVector);

  ItemRefVector::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new Item(name.str()));
    itemvector->push_back(item);
  }

  val = 20;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector has invalid size");

  ItemRefVector::iterator i = itemvector->begin();

  int ival = 0;
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");

  i += 4;
  ival = 4;
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");

  i = itemvector->erase(i);
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");
  UNIT_ASSERT_EQUAL((int)itemvector->size(), 19, "itemvector size isn't valid");

  i = itemvector->erase(i, itemvector->end());

  UNIT_ASSERT_TRUE(i == itemvector->end(), "iterator isn't expected end iterator");
  UNIT_ASSERT_EQUAL((int)itemvector->size(), 4, "itemvector size isn't valid");

  itemvector->clear();

  UNIT_ASSERT_EQUAL((int)itemvector->size(), 0, "itemvector size isn't valid");
  UNIT_ASSERT_TRUE(itemvector->empty(), "itemvector must be empty");
}

void ObjectVectorTestUnit::test_int_vector()
{
  typedef object_ptr<IntVector> itemvector_ptr;

  itemvector_ptr itemvector = ostore_.insert(new IntVector);

  IntVector::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    itemvector->push_back(i);
  }

  val = 20;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector has invalid size");

  IntVector::iterator i = itemvector->begin();

  int ival = 0;
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");

  i += 4;
  ival = 4;
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");

  i = itemvector->erase(i);
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");
  UNIT_ASSERT_EQUAL((int)itemvector->size(), 19, "itemvector size isn't valid");

  i = itemvector->erase(i, itemvector->end());

  UNIT_ASSERT_TRUE(i == itemvector->end(), "iterator isn't expected end iterator");
  UNIT_ASSERT_EQUAL((int)itemvector->size(), 4, "itemvector size isn't valid");

  itemvector->clear();

  UNIT_ASSERT_EQUAL((int)itemvector->size(), 0, "itemvector size isn't valid");
  UNIT_ASSERT_TRUE(itemvector->empty(), "itemvector must be empty");
}

void ObjectVectorTestUnit::test_ptr_vector()
{
  typedef object_ptr<ItemPtrVector> itemvector_ptr;
  typedef ItemPtrVector::value_type item_ptr;

  itemvector_ptr itemvector = ostore_.insert(new ItemPtrVector);

  ItemPtrVector::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new Item(name.str()));
    itemvector->push_back(item);
  }

  val = 20;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector has invalid size");

  ItemPtrVector::iterator i = itemvector->begin();

  int ival = 0;
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");

  i += 4;
  ival = 4;
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");

  i = itemvector->erase(i);
  UNIT_ASSERT_EQUAL((int)(*i)->index(), ival, "item is invalid");
  UNIT_ASSERT_EQUAL((int)itemvector->size(), 19, "itemvector size isn't valid");

  i = itemvector->erase(i, itemvector->end());

  UNIT_ASSERT_TRUE(i == itemvector->end(), "iterator isn't expected end iterator");
  UNIT_ASSERT_EQUAL((int)itemvector->size(), 4, "itemvector size isn't valid");

  itemvector->clear();

  UNIT_ASSERT_EQUAL((int)itemvector->size(), 0, "itemvector size isn't valid");
  UNIT_ASSERT_TRUE(itemvector->empty(), "itemvector must be empty");
}
