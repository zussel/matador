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
  add_test("ptr_vector", std::tr1::bind(&ObjectVectorTestUnit::test_ptr_vector, this), "test object vector with pointers");
}

ObjectVectorTestUnit::~ObjectVectorTestUnit()
{}

void ObjectVectorTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<ContainerItem<ItemPtrVector> >, "ITEM_PTR");
  ostore_.insert_prototype(new object_producer<ItemPtrVector>, "ITEM_PTR_VECTOR");
}

void ObjectVectorTestUnit::finalize()
{
}

void ObjectVectorTestUnit::test_ref_vector()
{
}

void ObjectVectorTestUnit::test_ptr_vector()
{
  typedef object_ptr<ItemPtrVector> itemvector_ptr;
  typedef object_ptr<ItemPtrVector::value_type> item_ptr;

  itemvector_ptr itemvector = ostore_.insert(new ItemPtrVector);

  ItemPtrVector::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new ItemPtrVector::value_type(name.str()));
    itemvector->push_back(item);
  }

  val = 20;
  UNIT_ASSERT_EQUAL(itemvector->size(), val, "pointer vector has invalid size");

  ItemPtrVector::iterator i = itemvector->begin() + 4;

  int ival = 4;
  UNIT_ASSERT_EQUAL((*i)->index(), ival, "item is invalid");

  i = itemvector->erase(i);
  UNIT_ASSERT_EQUAL((*i)->index(), ival, "item is invalid");

/*
  cout << "items of vector\n";
  for (ItemPtrVector::const_iterator i = itemvector->begin(); i != itemvector->end(); ++i) {
    cout << "item [" << i->get()->get_string() << "] (index: " << i->get()->index() << ")\n";
  }
*/
}
