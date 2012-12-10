#include "ObjectListTestUnit.hpp"

#include "Item.hpp"

#include "object/object_list.hpp"
#include "object/object_view.hpp"
#include "object/object_serializer.hpp"

#include "tools/byte_buffer.hpp"

#include <fstream>

using namespace oos;
using namespace std;

ObjectListTestUnit::ObjectListTestUnit()
  : unit_test("object list")
{
  add_test("int", std::tr1::bind(&ObjectListTestUnit::test_int_list, this), "test object list with integers");
  add_test("ref", std::tr1::bind(&ObjectListTestUnit::test_ref_list, this), "test object list with references");
  add_test("ptr", std::tr1::bind(&ObjectListTestUnit::test_ptr_list, this), "test object list with pointers");
  add_test("linked_int", std::tr1::bind(&ObjectListTestUnit::test_linked_int_list, this), "test linked integer list");
  add_test("linked_ref", std::tr1::bind(&ObjectListTestUnit::test_linked_ref_list, this), "test linked object list with references");
  add_test("linked_ptr", std::tr1::bind(&ObjectListTestUnit::test_linked_ptr_list, this), "test linked object list with pointers");
  add_test("direct_ref", std::tr1::bind(&ObjectListTestUnit::test_direct_ref_list, this), "test object list without relation table");
}

ObjectListTestUnit::~ObjectListTestUnit()
{}

void
ObjectListTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("ITEM");
  ostore_.insert_prototype<IntList>("INT_LIST");
  ostore_.insert_prototype<ItemRefList>("ITEM_REF_LIST");
  ostore_.insert_prototype<ItemPtrList>("ITEM_PTR_LIST");
  ostore_.insert_prototype<LinkedItemPtrList>("LINKED_ITEM_PTR_LIST");
  ostore_.insert_prototype<LinkedItemRefList>("LINKED_ITEM_REF_LIST");
  ostore_.insert_prototype<LinkedIntList>("LINKED_INT_LIST");
  
  ostore_.insert_prototype<person>("person", true);
  ostore_.insert_prototype<employee, person>("employee");
  ostore_.insert_prototype<department>("department");
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

  intlist_ptr intlist = ostore_.insert(new IntList);

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

  itemlist_ptr itemlist = ostore_.insert(new ItemRefList);

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

    itemlist_ptr itemlist = ostore_.insert(new ItemPtrList);

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
  
  intlist_ptr intlist = ostore_.insert(new LinkedIntList);

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

  long id_val = (*i)->id();

  i = intlist->erase(i);

  UNIT_ASSERT_NOT_EQUAL((*i)->id(), id_val, "returned iterator is the same as erased");
  UNIT_ASSERT_EQUAL((int)intlist->size(), 4, "linked list size is invalid");
  
  // clear list
  intlist->clear();

  UNIT_ASSERT_EQUAL((int)intlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(intlist->empty(), "linked item list must be empty");

  UNIT_ASSERT_TRUE(ostore_.remove(intlist), "couldn't remove linked item list");
}

void
ObjectListTestUnit::test_linked_ref_list()
{
  typedef object_ptr<LinkedItemRefList> itemlist_ptr;
  typedef LinkedItemRefList::value_type item_ptr;
  
  itemlist_ptr itemlist = ostore_.insert(new LinkedItemRefList);

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

  long id_val = (*i)->id();

  i = itemlist->erase(i);

  UNIT_ASSERT_NOT_EQUAL((*i)->id(), id_val, "returned iterator is the same as erased");
  UNIT_ASSERT_EQUAL((int)itemlist->size(), 4, "linked list size is invalid");
  
  // clear list
  itemlist->clear();

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(itemlist->empty(), "linked item list must be empty");

  UNIT_ASSERT_TRUE(ostore_.remove(itemlist), "couldn't remove linked item list");
}

void
ObjectListTestUnit::test_linked_ptr_list()
{
  typedef object_ptr<LinkedItemPtrList> itemlist_ptr;
  typedef LinkedItemPtrList::value_type item_ptr;
  
  itemlist_ptr itemlist = ostore_.insert(new LinkedItemPtrList);

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

  long id_val = (*i)->id();

  i = itemlist->erase(i);

  UNIT_ASSERT_NOT_EQUAL((*i)->id(), id_val, "returned iterator is the same as erased");
  UNIT_ASSERT_EQUAL((int)itemlist->size(), 4, "linked list size is invalid");
  
  // clear list
  itemlist->clear();

  UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "linked list is not empty");
  UNIT_ASSERT_TRUE(itemlist->empty(), "linked item list must be empty");

  UNIT_ASSERT_TRUE(ostore_.remove(itemlist), "couldn't remove linked item list");
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
