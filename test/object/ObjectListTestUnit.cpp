#include "ObjectListTestUnit.hpp"

#include "../Item.hpp"

#include "object/object_view.hpp"

#include <fstream>

using namespace oos;
using namespace std;

ObjectListTestUnit::ObjectListTestUnit()
  : unit_test("list", "serializable list")
{
  add_test("int", std::bind(&ObjectListTestUnit::test_int_list, this), "test serializable list with integers");
  add_test("ref", std::bind(&ObjectListTestUnit::test_ref_list, this), "test serializable list with references");
  add_test("ptr", std::bind(&ObjectListTestUnit::test_ptr_list, this), "test serializable list with pointers");
  add_test("linked_int", std::bind(&ObjectListTestUnit::test_linked_int_list, this), "test linked integer list");
  add_test("linked_ref", std::bind(&ObjectListTestUnit::test_linked_ref_list, this), "test linked serializable list with references");
  add_test("linked_ptr", std::bind(&ObjectListTestUnit::test_linked_ptr_list, this), "test linked serializable list with pointers");
  add_test("direct_ref", std::bind(&ObjectListTestUnit::test_direct_ref_list, this), "test serializable list without relation table");
}

ObjectListTestUnit::~ObjectListTestUnit()
{}

void
ObjectListTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("ITEM");
  ostore_.insert_prototype(new list_object_producer<IntList>("int_list"), "INT_LIST");
  ostore_.insert_prototype(new list_object_producer<ItemRefList>("ref_list"), "ITEM_REF_LIST");
  ostore_.insert_prototype(new list_object_producer<ItemPtrList>("ptr_list"), "ITEM_PTR_LIST");
  ostore_.insert_prototype(new list_object_producer<LinkedIntList>("linked_int_list"), "LINKED_INT_LIST");
  ostore_.insert_prototype(new list_object_producer<LinkedItemPtrList>("linked_ptr_list"), "LINKED_ITEM_PTR_LIST");
  ostore_.insert_prototype(new list_object_producer<LinkedItemRefList>("linked_ref_list"), "LINKED_ITEM_REF_LIST");

  ostore_.insert_prototype<person>("person", true);
  ostore_.insert_prototype<department>("department");
  ostore_.insert_prototype<employee, person>("employee");
}

void
ObjectListTestUnit::finalize()
{
  ostore_.clear(true);
}


void
ObjectListTestUnit::test_int_list()
{
  typedef object_ptr<IntList> intlist_ptr;

  intlist_ptr intlist = ostore_.insert(new IntList("int_list"));

  IntList::size_type val = 0;
  UNIT_ASSERT_EQUAL(intlist->size(), val, "integer list is not empty");

  for (int i = 0; i < 10; ++i) {
    intlist->push_back(i);
  }

  val = 10;
  UNIT_ASSERT_EQUAL(intlist->size(), val, "integer list has invalid size");
  
  int count = 0;
  for (IntList::const_iterator i = intlist->begin(); i != intlist->end(); ++i) {
    UNIT_ASSERT_LESS(count, 10, "item list count isn't valid");
    ++count;
  }
}

void
ObjectListTestUnit::test_ref_list()
{
  typedef object_ptr<ItemRefList> itemlist_ptr;
  typedef ItemRefList::value_type item_ptr;

  itemlist_ptr itemlist = ostore_.insert(new ItemRefList("ref_list"));

  ItemRefList::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list is not empty");

  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new Item(name.str()));
    if (i % 2) {
      itemlist->push_back(item);
    }
  }

  val = 10;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list has invalid size");

  typedef object_view<Item> item_view_t;
  item_view_t item_view(ostore_);

  val = 20;
  UNIT_ASSERT_EQUAL(item_view.size(), val, "item view has invalid size");


  item_view_t::iterator first = item_view.begin();
  item_view_t::iterator last = item_view.end();

  int count = 0;
  while (first != last) {
    UNIT_ASSERT_LESS(count, 20, "item view count isn't valid");
    ++first;
    ++count;
  }
  
  count = 0;
  for (ItemRefList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
    UNIT_ASSERT_LESS(count, 10, "item list count isn't valid");
    ++count;
  }  
}

void
ObjectListTestUnit::test_ptr_list()
{
  try {
    typedef object_ptr<ItemPtrList> itemlist_ptr;
    typedef ItemPtrList::value_type item_ptr;

    itemlist_ptr itemlist = ostore_.insert(new ItemPtrList("ptr_list"));

    ItemPtrList::size_type val = 0;
    UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list is not empty");

    for (int i = 0; i < 20; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));
      if (i % 2) {
        itemlist->push_back(item);
      }
    }

    val = 10;
    UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list has invalid size");

    typedef object_view<Item> item_view_t;
    item_view_t item_view(ostore_);

    val = 20;
    UNIT_ASSERT_EQUAL(item_view.size(), val, "item view has invalid size");


    item_view_t::iterator first = item_view.begin();
    item_view_t::iterator last = item_view.end();

    int count = 0;
    while (first != last) {
      UNIT_ASSERT_LESS(count, 20, "item view count isn't valid");
      ++first;
      ++count;
    }
    
    count = 0;
    for (ItemPtrList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
      UNIT_ASSERT_LESS(count, 10, "item list count isn't valid");
      ++count;
    }
  } catch (exception &ex) {
    UNIT_FAIL(ex.what());
  }
}

void
ObjectListTestUnit::test_linked_int_list()
{
  typedef object_ptr<LinkedIntList> intlist_ptr;
  
  intlist_ptr intlist = ostore_.insert(new LinkedIntList("linked_int_list"));

  UNIT_ASSERT_EQUAL((int)intlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(intlist->empty(), "linked item list must be empty");

  intlist->push_back(5);
  intlist->push_back(8);
  intlist->push_back(1);
  intlist->push_back(7);

  UNIT_ASSERT_FALSE(intlist->empty(), "linked item list couldn't be empty");
  UNIT_ASSERT_EQUAL((int)intlist->size(), 4, "linked list size is invalid");

  intlist->push_front(0);

  UNIT_ASSERT_EQUAL((int)intlist->size(), 5, "linked list size is invalid");

  // remove an item
  LinkedIntList::iterator i = intlist->begin();

  unsigned long id_val = (*i)->id();

  i = intlist->erase(i);

  UNIT_ASSERT_NOT_EQUAL((*i)->id(), id_val, "returned iterator is the same as erased");
  UNIT_ASSERT_EQUAL((int)intlist->size(), 4, "linked list size is invalid");
  
  // clear list
  intlist->clear();

  UNIT_ASSERT_EQUAL((int)intlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(intlist->empty(), "linked item list must be empty");

  UNIT_ASSERT_TRUE(ostore_.is_removable(intlist), "couldn't remove linked item list");
  
  ostore_.remove(intlist);
}

void
ObjectListTestUnit::test_linked_ref_list()
{
  typedef object_ptr<LinkedItemRefList> itemlist_ptr;
  typedef LinkedItemRefList::value_type item_ptr;
  
  itemlist_ptr itemlist = ostore_.insert(new LinkedItemRefList("linked_ref_list"));

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(itemlist->empty(), "linked item list must be empty");

  item_ptr item = ostore_.insert(new Item("Schrank"));
  itemlist->push_back(item);
  item = ostore_.insert(new Item("Tisch"));
  itemlist->push_back(item);
  item = ostore_.insert(new Item("Stuhl"));
  itemlist->push_back(item);
  item = ostore_.insert(new Item("Bett"));
  itemlist->push_back(item);

  UNIT_ASSERT_FALSE(itemlist->empty(), "linked item list couldn't be empty");
  UNIT_ASSERT_EQUAL((int)itemlist->size(), 4, "linked list size is invalid");

  item = ostore_.insert(new Item("Teppich"));
  itemlist->push_front(item);

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 5, "linked list size is invalid");

  // remove an item
  LinkedItemRefList::iterator i = itemlist->begin();

  unsigned long id_val = (*i)->id();

  i = itemlist->erase(i);

  UNIT_ASSERT_NOT_EQUAL((*i)->id(), id_val, "returned iterator is the same as erased");
  UNIT_ASSERT_EQUAL((int)itemlist->size(), 4, "linked list size is invalid");
  
  // clear list
  itemlist->clear();

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(itemlist->empty(), "linked item list must be empty");

  UNIT_ASSERT_TRUE(ostore_.is_removable(itemlist), "couldn't remove linked item list");
  
  ostore_.remove(itemlist);
}

void
ObjectListTestUnit::test_linked_ptr_list()
{
  typedef object_ptr<LinkedItemPtrList> itemlist_ptr;
  typedef LinkedItemPtrList::value_type item_ptr;
  
  itemlist_ptr itemlist = ostore_.insert(new LinkedItemPtrList("linked_ptr_list"));

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(itemlist->empty(), "linked item list must be empty");

  item_ptr item = ostore_.insert(new Item("Schrank"));
  itemlist->push_back(item);
  item = ostore_.insert(new Item("Tisch"));
  itemlist->push_back(item);
  item = ostore_.insert(new Item("Stuhl"));
  itemlist->push_back(item);
  item = ostore_.insert(new Item("Bett"));
  itemlist->push_back(item);

  UNIT_ASSERT_FALSE(itemlist->empty(), "linked item list couldn't be empty");
  UNIT_ASSERT_EQUAL((int)itemlist->size(), 4, "linked list size is invalid");

  item = ostore_.insert(new Item("Teppich"));
  itemlist->push_front(item);

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 5, "linked list size is invalid");

  // remove an item
  LinkedItemPtrList::iterator i = itemlist->begin();

  unsigned long id_val = (*i)->id();

  i = itemlist->erase(i);

  UNIT_ASSERT_NOT_EQUAL((*i)->id(), id_val, "returned iterator is the same as erased");
  UNIT_ASSERT_EQUAL((int)itemlist->size(), 4, "linked list size is invalid");
  
  // clear list
  itemlist->clear();

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(itemlist->empty(), "linked item list must be empty");

  UNIT_ASSERT_TRUE(ostore_.is_removable(itemlist), "couldn't remove linked item list");
  
  ostore_.remove(itemlist);
}

void ObjectListTestUnit::test_direct_ref_list()
{
  typedef object_ptr<department> department_ptr;
  typedef object_ptr<employee> employee_ptr;
  
  department_ptr dep = ostore_.insert(new department("development"));
  
  UNIT_ASSERT_TRUE(dep->id() > 0, "department is must be greater zero");
  UNIT_ASSERT_EQUAL((int)dep->size(), 0, "department is not empty");
  UNIT_ASSERT_TRUE(dep->empty(), "department must be empty");
  
  employee_ptr emp1 = ostore_.insert(new employee("Karl"));
  
  UNIT_ASSERT_TRUE(emp1->dep().id() == 0, "department id of employee must be zero");

  dep->add(emp1);

  UNIT_ASSERT_TRUE(emp1->dep().id() > 0, "department id of employee must be zero");
  UNIT_ASSERT_EQUAL(emp1->dep()->name(), "development", "department id of employee must be zero");
    
  department::iterator i = dep->begin();
  
  UNIT_ASSERT_EQUAL((*i)->name(), "Karl", "employees name must be 'Karl'");
  
  // remove karl from department
  i = dep->erase(i);
  
  UNIT_ASSERT_TRUE(i == dep->end(), "iterator must be end");
  UNIT_ASSERT_TRUE(emp1->dep().id() == 0, "department id of employee must be zero");
}

void ObjectListTestUnit::test_many_to_many()
{

}
