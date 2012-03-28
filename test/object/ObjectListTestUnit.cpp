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
  cout << "inserting 20 items\n";
  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new ItemRefList::value_type(name.str()));
    cout << "inserted item [" << item->get_string() << "]\n";
  }
  cout << "inserting item reference list\n";
  itemlist_ptr itemlist = ostore_.insert(new ItemRefList);
  cout << "item list [" << itemlist->id() << "]\n";
  cout << "add every second item as reference to item list\n";
  
  typedef object_view<ItemRefList::value_type> item_view_t;
  item_view_t item_view(ostore_);
  item_view_t::iterator first = item_view.begin();
  item_view_t::iterator last = item_view.end();
  while (first != last) {
    item_ptr i = (*first++);
    if (i->id() % 2) {
      cout << "adding item [" << i->get_string() << "(id: " << i->id() << ")]\n";
      itemlist->push_back(i);
    }
  }
  cout << "dumping items of item list\n";
  
  for (ItemRefList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
    cout << "item [" << i->get()->get_string() << "]\n";
  }
  
}

void
ObjectListTestUnit::test_ptr_list()
{
  typedef object_ptr<ItemPtrList> itemlist_ptr;
  typedef object_ptr<ItemPtrList::value_type> item_ptr;

  cout << "inserting item pointer list\n";
  itemlist_ptr itemlist = ostore_.insert(new ItemPtrList);
  cout << "item list [" << itemlist->id() << "]\n";

  cout << "inserting 20 items\n";
  for (int i = 0; i < 20; ++i) {
    stringstream name;
    name << "Item " << i+1;
    item_ptr item = ostore_.insert(new ItemPtrList::value_type(name.str()));
    itemlist->push_back(item);
  }

  cout << "items of list\n";
  for (ItemPtrList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
    cout << "item [" << i->get()->get_string() << "]\n";
  }
  
  cout << "items of view\n";
  typedef object_view<ItemPtrList::value_type> item_view_t;
  item_view_t item_view(ostore_);
  item_view_t::iterator first = item_view.begin();
  item_view_t::iterator last = item_view.end();
  while (first != last) {
    item_ptr i = (*first++);
    cout << "item [" << i->get_string() << "(id: " << i->id() << ")]\n";
  }
}

void
ObjectListTestUnit::test_linked_list()
{
  typedef object_ptr<LinkedItemList> itemlist_ptr;
  cout << "inserting linked item list\n";
  
  itemlist_ptr itemlist = ostore_.insert(new LinkedItemList);
  cout << "inserted linked item list (id: " << itemlist->id() << ")\n";
  /*
  out.open("itemlist0.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */
  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist1.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */

  cout << "add item to linked list\n";
  itemlist->push_back(new LinkedItem("Schrank"));
  itemlist->push_back(new LinkedItem("Tisch"));
  itemlist->push_back(new LinkedItem("Stuhl"));
  itemlist->push_back(new LinkedItem("Bett"));
  
  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist2.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */

  itemlist->push_front(new LinkedItem("Teppich"));

  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist3.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */

  cout << "dump linked list\n";

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
