#include "ObjectListTestUnit.hpp"

#include "object/object_list.hpp"

#include <fstream>

using namespace oos;
using namespace std;

class ItemPtrList;
class ItemRefList;

class Item : public object
{
public:
  Item() {}
  Item(const std::string &name) : name_(name) {}
  virtual ~Item() {}

	void read_from(object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_string("name", name_);
  }
	void write_to(object_atomizer *writer)
  {
    object::write_to(writer);
    writer->write_string("name", name_);
  }
  
  std::string name() const { return name_; }
  void name(const std::string &n)
  { 
    mark_modified();
    name_ = n;
  }

  object_ref<ItemPtrList> itemlist() const { return list_; }
  void itemlist(const object_ref<ItemPtrList> &l)
  {
    mark_modified();
    list_ = l;
  }

private:
  std::string name_;
  object_ref<ItemPtrList> list_;
};

class ItemPtrList : public object
{
public:
  typedef object_ptr_list<Item> item_list_t;
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;

public:
  ItemPtrList() {}
  virtual ~ItemPtrList() {}

	void read_from(object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_list("item_list", item_list_);
  }
	void write_to(object_atomizer *writer)
  {
    object::write_to(writer);
    writer->write_object_list("item_list", item_list_);
  }
  
  void push_front(Item *i) { item_list_.push_front(i, this); }
  void push_back(Item *i) { item_list_.push_back(i, this); }

  iterator begin() { return item_list_.begin(); }
  const_iterator begin() const { return item_list_.begin(); }

  iterator end() { return item_list_.end(); }
  const_iterator end() const { return item_list_.end(); }

  bool empty() const { return item_list_.empty(); }
  void clear() { item_list_.clear(); }

private:
  item_list_t item_list_;
};

class LinkedItem : public object_list_node<LinkedItem>
{
public:
  LinkedItem() {}
  LinkedItem(const std::string &name) : name_(name) {}
  virtual ~LinkedItem() {}

	void read_from(object_atomizer *reader)
  {
    object_list_node::read_from(reader);
    reader->read_string("name", name_);
  }
	void write_to(object_atomizer *writer)
  {
    object_list_node::write_to(writer);
    writer->write_string("name", name_);
  }
  
  std::string name() const { return name_; }
  void name(const std::string &n) { name_ = n; }

private:
  std::string name_;
};

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
  ostore_.insert_prototype(new object_producer<Item>, "ITEM");
  ostore_.insert_prototype(new object_producer<LinkedItem>, "LINKED_ITEM");
}

void
ObjectListTestUnit::finalize()
{
  ostore_.clear();
}

void
ObjectListTestUnit::test_ref_list()
{
}

void
ObjectListTestUnit::test_ptr_list()
{
}

void
ObjectListTestUnit::test_linked_list()
{
  typedef linked_object_list<LinkedItem> LinkedItemList;

  std::ofstream out;
  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist0.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */
  LinkedItemList linked_list;
  ostore_.insert(linked_list);

  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist1.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */

  linked_list.push_back(new LinkedItem("Schrank"));
  linked_list.push_back(new LinkedItem("Tisch"));
  linked_list.push_back(new LinkedItem("Stuhl"));
  linked_list.push_back(new LinkedItem("Bett"));
  
  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist2.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */


  linked_list.push_front(new LinkedItem("Teppich"));

  ostore_.dump_objects(std::cout);
  /*
  out.open("itemlist3.dot");
  ostore_.dump_prototypes(out);
  out.close();
  */

  LinkedItemList::const_iterator first = linked_list.begin();
  LinkedItemList::const_iterator last = linked_list.end();

  while (first != last) {
    cout << "item name: " << first->name() << "\n";
    ++first;
  }

  // remove an item
  LinkedItemList::iterator i = linked_list.begin();
  cout << "item to remove: " << i->name() << "\n";
  i = linked_list.erase(i);
  cout << "next item name: " << i->name() << "\n";

  first = linked_list.begin();
  while (first != last) {
    cout << "item name: " << first->name() << "\n";
    ++first;
  }
  
  // clear list
  std::cout << "clear list\n";
  linked_list.clear();
  first = linked_list.begin();
  while (first != last) {
    cout << "item name: " << first->name() << "\n";
    ++first;
  }
  
  ostore_.dump_objects(std::cout);
  std::cout << "remove list from object store ... ";
  if (ostore_.remove(linked_list)) {
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
