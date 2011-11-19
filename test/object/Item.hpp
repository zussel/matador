#ifndef ITEM_HPP
#define ITEM_HPP

#include "object/object.hpp"
#include "object/object_list.hpp"
#include "object/object_atomizer.hpp"

template < class L >
class Item : public oos::object
{
public:
  Item() {}
  Item(const std::string &name) : name_(name) {}
  virtual ~Item() {}

	void read_from(oos::object_atomizer *reader)
  {
    oos::object::read_from(reader);
    reader->read_string("name", name_);
    reader->read_object("itemlist", list_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    oos::object::write_to(writer);
    writer->write_string("name", name_);
    writer->write_object("itemlist", list_);
  }
  
  std::string name() const { return name_; }
  void name(const std::string &n)
  { 
    mark_modified();
    name_ = n;
  }

  oos::object_ref<L> itemlist() const { return list_; }
  void itemlist(const oos::object_ref<L> &l)
  {
    mark_modified();
    list_ = l;
  }

private:
  std::string name_;
  oos::object_ref<L> list_;
};

class ItemPtrList : public oos::object
{
public:
  typedef oos::object_ptr_list<Item<ItemPtrList> > item_list_t;
  typedef Item<ItemPtrList> value_type;
  typedef ItemPtrList self;
  typedef oos::object_ref<self> self_ref;
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;

public:
  ItemPtrList()
    : item_list_(this, "itemlist")
  {}
  virtual ~ItemPtrList() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_list("item_list", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
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

  size_t size() { return item_list_.size(); }

private:
  item_list_t item_list_;
};

class ItemRefList : public oos::object
{
public:
  typedef oos::object_ref_list<Item<ItemRefList> > item_list_t;
  typedef item_list_t::value_type value_type;
  typedef item_list_t::value_type_ref value_type_ref;
  typedef ItemRefList self;
  typedef oos::object_ref<self> self_ref;
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;

public:
  ItemRefList()
    : item_list_(this, "itemlist")
  {}
  virtual ~ItemRefList() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_list("item_list", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
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

class LinkedItem : public oos::object_list_node<LinkedItem>
{
public:
  LinkedItem() {}
  LinkedItem(const std::string &name) : name_(name) {}
  virtual ~LinkedItem() {}

	void read_from(oos::object_atomizer *reader)
  {
    object_list_node::read_from(reader);
    reader->read_string("name", name_);
    reader->read_object("itemlist", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
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

  oos::object_ref<LinkedItemList> item_list() const { return item_list_; }
  void item_list(const oos::object_ref<LinkedItemList> &il)
  {
    mark_modified();
    item_list_ = il;
  }

private:
  std::string name_;
  oos::object_ref<LinkedItemList> item_list_;
};

class LinkedItemList : public oos::object
{
  typedef oos::linked_object_list<LinkedItem> item_list_t;
  typedef item_list_t::value_type value_type;
  typedef oos::object_ref<value_type> value_type_ref;
  typedef LinkedItemList self;
  typedef oos::object_ref<self> self_ref;

public:
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;

public:
  LinkedItemList()
    : item_list_(this, "itemlist")
  {}
  virtual ~LinkedItemList() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_list("item_list", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
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

#endif /* ITEM_HPP */
