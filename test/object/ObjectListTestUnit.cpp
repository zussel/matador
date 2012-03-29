#include "ObjectListTestUnit.hpp"

#include "Item.hpp"

#include "object/object_list.hpp"
#include "object/object_view.hpp"

#include <fstream>

using namespace oos;
using namespace std;

ObjectListTestUnit::ObjectListTestUnit()
  : unit_test("object list")
{
  add_test("ref_list", std::tr1::bind(&ObjectListTestUnit::test_ref_list, this), "test object list with references");
  add_test("ptr_list", std::tr1::bind(&ObjectListTestUnit::test_ptr_list, this), "test object list with pointers");
  add_test("linked_list", std::tr1::bind(&ObjectListTestUnit::test_linked_list, this), "test linked object list");
  add_test("linked_ref_list", std::tr1::bind(&ObjectListTestUnit::test_linked_ref_list, this), "test linked object list with references");
  add_test("linked_ptr_list", std::tr1::bind(&ObjectListTestUnit::test_linked_ptr_list, this), "test linked object list with pointers");
}

ObjectListTestUnit::~ObjectListTestUnit()
{}

void
ObjectListTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<ContainerItem<ItemRefList> >, "ITEM_REF");
  ostore_.insert_prototype(new object_producer<ContainerItem<ItemPtrList> >, "ITEM_PTR");
  ostore_.insert_prototype(new object_producer<ItemRefList>, "ITEM_REF_LIST");
  ostore_.insert_prototype(new object_producer<ItemPtrList>, "ITEM_PTR_LIST");
  ostore_.insert_prototype(new object_producer<LinkedItem>, "LINKED_ITEM");
  ostore_.insert_prototype(new object_producer<LinkedItemList>, "LINKED_ITEM_LIST");
}

void
ObjectListTestUnit::finalize()
{
  ostore_.clear();
}

void
ObjectListTestUnit::test_ref_list()
{
  typedef object_ptr<ItemRefList> itemlist_ptr;
  typedef object_ptr<ItemRefList::value_type> item_ptr;

  itemlist_ptr itemlist = ostore_.insert(new ItemRefList);

  ItemRefList::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list is not empty");

  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new ItemRefList::value_type(name.str()));
    if (i % 2) {
      itemlist->push_back(item);
    }
  }

  val = 10;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list has invalid size");

  typedef object_view<ItemRefList::value_type> item_view_t;
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
  typedef object_ptr<ItemPtrList> itemlist_ptr;
  typedef object_ptr<ItemPtrList::value_type> item_ptr;

  itemlist_ptr itemlist = ostore_.insert(new ItemPtrList);

  ItemPtrList::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list is not empty");

  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new ItemPtrList::value_type(name.str()));
    if (i % 2) {
      itemlist->push_back(item);
    }
  }

  val = 10;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "reference list has invalid size");

  typedef object_view<ItemPtrList::value_type> item_view_t;
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
}

void
ObjectListTestUnit::test_linked_list()
{
  typedef object_ptr<LinkedItemList> itemlist_ptr;
  
  itemlist_ptr itemlist = ostore_.insert(new LinkedItemList);

  LinkedItemList::size_type val = 0;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "linked list is not empty");

  itemlist->push_back(new LinkedItem("Schrank"));
  itemlist->push_back(new LinkedItem("Tisch"));
  itemlist->push_back(new LinkedItem("Stuhl"));
  itemlist->push_back(new LinkedItem("Bett"));

  val = 4;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "linked list size is invalid");

  itemlist->push_front(new LinkedItem("Teppich"));

  val = 5;
  UNIT_ASSERT_EQUAL(itemlist->size(), val, "linked list size is invalid");

  LinkedItemList::const_iterator first = itemlist->begin();
  LinkedItemList::const_iterator last = itemlist->end();

  while (first != last) {
    cout << "item name: " << first->name() << " (id: " << first->id() << ")\n";
    ++first;
  }
  cout << "remove item from linked list\n";

  // remove an item
  LinkedItemList::iterator i = itemlist->begin();
  cout << "item to remove: " << i->name() << "\n";
  i = itemlist->erase(i);
  cout << "next item name: " << i->name() << "\n";

  ostore_.dump_objects(std::cout);

  cout << "dump linked list\n";
  first = itemlist->begin();
  while (first != last) {
    cout << "item name: " << first->name() << " (id: " << first->id() << ")\n";
    ++first;
  }
  
  // clear list
  std::cout << "clear list\n";
  itemlist->clear();
  first = itemlist->begin();
  while (first != last) {
    cout << "item name: " << first->name() << " (id: " << first->id() << ")\n";
    ++first;
  }
  
  ostore_.dump_objects(std::cout);
  std::cout << "remove list from object store ... ";
  if (ostore_.remove(itemlist)) {
    std::cout << "succeeded!\n";
  } else {
    std::cout << "failed!\n";
  }
  ostore_.dump_objects(std::cout);
}

void
ObjectListTestUnit::test_linked_ref_list()
{
}

void
ObjectListTestUnit::test_linked_ptr_list()
{
}
