#include "ObjectListTestUnit.hpp"

#include "object/object_list.hpp"
#include "object/object_view.hpp"

#include <fstream>

using namespace oos;
using namespace std;

template < class L >
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
    reader->read_object("itemlist", list_);
  }
	void write_to(object_atomizer *writer)
  {
    object::write_to(writer);
    writer->write_string("name", name_);
    writer->write_object("itemlist", list_);
  }
  
  std::string name() const { return name_; }
  void name(const std::string &n)
  { 
    mark_modified();
    name_ = n;
  }

  object_ref<L> itemlist() const { return list_; }
  void itemlist(const object_ref<L> &l)
  {
    mark_modified();
    list_ = l;
  }

private:
  std::string name_;
  object_ref<L> list_;
};

class ItemPtrList : public object
{
public:
  typedef object_ptr_list<Item<ItemPtrList> > item_list_t;
  typedef Item<ItemPtrList> value_type;
  typedef ItemPtrList self;
  typedef object_ref<self> self_ref;
  typedef typename item_list_t::iterator iterator;
  typedef typename item_list_t::const_iterator const_iterator;

public:
  ItemPtrList()
    : item_list_("itemlist")
  {}
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
  
  void push_front(value_type *i) { item_list_.push_front(i, self_ref(this)); }
  void push_back(value_type *i) { item_list_.push_back(i, self_ref(this)); }

  iterator begin() { return item_list_.begin(); }
  const_iterator begin() const { return item_list_.begin(); }

  iterator end() { return item_list_.end(); }
  const_iterator end() const { return item_list_.end(); }

  bool empty() const { return item_list_.empty(); }
  void clear() { item_list_.clear(); }

private:
  item_list_t item_list_;
};

class ItemRefList : public object
{
public:
  typedef object_ref_list<Item<ItemRefList> > item_list_t;
  typedef item_list_t::value_type value_type;
  typedef item_list_t::value_type_ref value_type_ref;
  typedef ItemRefList self;
  typedef object_ref<self> self_ref;
  typedef typename item_list_t::iterator iterator;
  typedef typename item_list_t::const_iterator const_iterator;

public:
  ItemRefList()
    : item_list_("itemlist")
  {}
  virtual ~ItemRefList() {}

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
  
  void push_front(const value_type_ref &i)
  {
    item_list_.push_front(i, self_ref(this));
  }
  void push_back(const value_type_ref &i)
  {
    item_list_.push_back(i, self_ref(this));
  }

  iterator begin() { return item_list_.begin(); }
  const_iterator begin() const { return item_list_.begin(); }

  iterator end() { return item_list_.end(); }
  const_iterator end() const { return item_list_.end(); }

  bool empty() const { return item_list_.empty(); }
  void clear() { item_list_.clear(); }

private:
  item_list_t item_list_;
};

class LinkedItemList;

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
    reader->read_object("itemlist", item_list_);
  }
	void write_to(object_atomizer *writer)
  {
    object_list_node::write_to(writer);
    writer->write_string("name", name_);
    writer->write_object("itemlist", item_list_);
  }
  
  std::string name() const { return name_; }
  void name(const std::string &n)
  {
    mark_modified();
    name_ = n;
  }

  object_ref<LinkedItemList> item_list() const { return item_list_; }
  void item_list(const object_ref<LinkedItemList> &il)
  {
    mark_modified();
    item_list_ = il;
  }

private:
  std::string name_;
  object_ref<LinkedItemList> item_list_;
};

class LinkedItemList : public object
{
  typedef linked_object_list<LinkedItem> item_list_t;
  typedef item_list_t::value_type value_type;
  typedef object_ref<value_type> value_type_ref;
  typedef LinkedItemList self;
  typedef object_ref<self> self_ref;

public:
  typedef typename item_list_t::iterator iterator;
  typedef typename item_list_t::const_iterator const_iterator;

public:
  LinkedItemList()
    : item_list_("itemlist")
  {}
  virtual ~LinkedItemList() {}

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
  
  void push_front(value_type *i)
  {
    item_list_.push_front(i, self_ref(this));
  }
  void push_back(value_type *i)
  {
    item_list_.push_back(i, self_ref(this));
  }

  iterator begin() { return item_list_.begin(); }
  const_iterator begin() const { return item_list_.begin(); }

  iterator end() { return item_list_.end(); }
  const_iterator end() const { return item_list_.end(); }

  bool empty() const { return item_list_.empty(); }
  void clear() { item_list_.clear(); }

  iterator erase(iterator i)
  {
    return item_list_.erase(i);
  }
private:
  item_list_t item_list_;
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
  ostore_.insert_prototype(new object_producer<Item<ItemRefList> >, "ITEM_REF");
  ostore_.insert_prototype(new object_producer<Item<ItemPtrList> >, "ITEM_PTR");
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
    cout << "inserted item [" << item->name() << "]\n";
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
      cout << "adding item [" << i->name() << "(id: " << i->id() << ")]\n";
      itemlist->push_back(i);
    }
  }
  cout << "dumping items of item list\n";
  
  for (ItemRefList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
    cout << "item [" << i->get()->name() << "]\n";
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
    itemlist->push_back(new ItemPtrList::value_type(name.str()));
  }

  cout << "items of list\n";
  for (ItemPtrList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
    cout << "item [" << i->get()->name() << "]\n";
  }
  
  cout << "items of view\n";
  typedef object_view<ItemPtrList::value_type> item_view_t;
  item_view_t item_view(ostore_);
  item_view_t::iterator first = item_view.begin();
  item_view_t::iterator last = item_view.end();
  while (first != last) {
    item_ptr i = (*first++);
    cout << "item [" << i->name() << "(id: " << i->id() << ")]\n";
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
    cout << "item name: " << first->name() << "\n";
    ++first;
  }
  cout << "remove item from linked list\n";

  // remove an item
  LinkedItemList::iterator i = itemlist->begin();
  cout << "item to remove: " << i->name() << "\n";
  i = itemlist->erase(i);
  cout << "next item name: " << i->name() << "\n";

  cout << "dump linked list\n";
  first = itemlist->begin();
  while (first != last) {
    cout << "item name: " << first->name() << "\n";
    ++first;
  }
  
  // clear list
  std::cout << "clear list\n";
  itemlist->clear();
  first = itemlist->begin();
  while (first != last) {
    cout << "item name: " << first->name() << "\n";
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
