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

  cout << "inserting item pointer vector\n";
  itemvector_ptr itemvector = ostore_.insert(new ItemPtrVector);
  cout << "item list [" << itemvector->id() << "]\n";

  cout << "inserting 20 items\n";
  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new ItemPtrVector::value_type(name.str()));
    itemvector->push_back(item);
  }

  cout << "items of vector\n";
  for (ItemPtrVector::const_iterator i = itemvector->begin(); i != itemvector->end(); ++i) {
    cout << "removing item [" << i->get()->get_string() << "] (index: " << i->get()->index() << ")\n";
  }

  ItemPtrVector::iterator i = itemvector->begin() + 4;

  cout << "item [" << i->get()->get_string() << "] (index: " << i->get()->index() << ")\n";
  i = itemvector->erase(i);

  cout << "items of vector\n";
  for (ItemPtrVector::const_iterator i = itemvector->begin(); i != itemvector->end(); ++i) {
    cout << "item [" << i->get()->get_string() << "] (index: " << i->get()->index() << ")\n";
  }

}
