#ifndef ITEM_HPP
#define ITEM_HPP

#include "object/object.hpp"
#include "object/object_list.hpp"
#include "object/object_vector.hpp"
#include "object/linked_object_list.hpp"
#include "object/object_atomizer.hpp"

#include "tools/varchar.hpp"

class AllBase : public oos::object
{
public:
  AllBase() {}
  virtual ~AllBase() {}

	void read_from(oos::object_atomizer *reader)
  {
    oos::object::read_from(reader);
    reader->read_char("val_char", char_);
    reader->read_float("val_float", float_);
    reader->read_double("val_double", double_);
    reader->read_int("val_int", int_);
    reader->read_long("val_long", long_);
    reader->read_unsigned("val_unsigned", unsigned_);
    reader->read_bool("val_bool", bool_);
    reader->read_string("val_string", string_);
    reader->read_varchar("val_varchar", varchar_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    oos::object::write_to(writer);
    writer->write_char("val_char", char_);
    writer->write_float("val_float", float_);
    writer->write_double("val_double", double_);
    writer->write_int("val_int", int_);
    writer->write_long("val_long", long_);
    writer->write_unsigned("val_unsigned", unsigned_);
    writer->write_bool("val_bool", bool_);
    writer->write_string("val_string", string_);
    writer->write_varchar("val_varchar", varchar_);
  }

  void set_char(char x) { modify(char_, x); }
  void set_float(float x) { modify(float_, x); }
  void set_double(double x) { modify(double_, x); }
  void set_int(int x) { modify(int_, x); }
  void set_long(long x) { modify(long_, x); }
  void set_unsigned(unsigned x) { modify(unsigned_, x); }
  void set_bool(bool x) { modify(bool_, x); }
  void set_string(const std::string &x) { modify(string_, x); }
  void set_varchar(const oos::varchar_base &x) { modify(varchar_, x); }

  char get_char() const { return char_; }
  float get_float() const { return float_; }
  double get_double() const { return double_; }
  int get_int() const { return int_; }
  long get_long() const { return long_; }
  unsigned get_unsigned() const { return unsigned_; }
  bool get_bool() const { return bool_; }
  std::string get_string() const { return string_; }
  oos::varchar_base get_varchar() const { return varchar_; }

private:
  char char_;
  float float_;
  double double_;
  int int_;
  long long_;
  unsigned unsigned_;
  bool bool_;
  std::string string_;
  oos::varchar<64> varchar_;
};

template < class L >
class Item : public oos::object
{
public:
  Item() {}
  Item(const std::string &name) : name_(name), index_(0) {}
  virtual ~Item() {}

	void read_from(oos::object_atomizer *reader)
  {
    oos::object::read_from(reader);
    reader->read_varchar("name", name_);
    reader->read_int("itemindex", index_);
    reader->read_object("itemlist", list_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    oos::object::write_to(writer);
    writer->write_varchar("name", name_);
    writer->write_int("itemindex", index_);
    writer->write_object("itemlist", list_);
  }
  
  std::string name() const { return name_.str(); }
  void name(const std::string &n)
  { 
    modify(name_, n);
  }

  int index() const { return index_; }
  void index(int i)
  { 
    modify(index_, i);
  }

  oos::object_ref<L> itemlist() const { return list_; }
  void itemlist(const oos::object_ref<L> &l)
  {
    modify(list_, l);
  }

private:
  oos::varchar<32> name_;
  int index_;
  oos::object_ref<L> list_;
};

class ItemPtrList : public oos::object
{
public:
  typedef Item<ItemPtrList> value_type;
  typedef oos::object_ptr_list<value_type> item_list_t;
  typedef item_list_t::value_type_ptr value_type_ptr;
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

  void push_front(const value_type_ptr &i)
  {
    item_list_.push_front(i);
  }

  void push_back(const value_type_ptr &i)
  {
    item_list_.push_back(i);
  }

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
    item_list_.push_front(i);
  }
  void push_back(const value_type_ref &i)
  {
    item_list_.push_back(i);
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

class LinkedItem : public oos::linked_object_list_node<LinkedItem>
{
public:
  LinkedItem() {}
  LinkedItem(const std::string &name) : name_(name) {}
  virtual ~LinkedItem() {}

	void read_from(oos::object_atomizer *reader)
  {
    linked_object_list_node::read_from(reader);
    reader->read_string("name", name_);
    reader->read_object("itemlist", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    linked_object_list_node::write_to(writer);
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
    item_list_.push_front(i);
  }

  void push_back(value_type *i)
  {
    item_list_.push_back(i);
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

class ItemPtrVector : public oos::object
{
public:
  typedef Item<ItemPtrVector> value_type;
  typedef oos::object_ptr_vector<value_type> item_vector_t;
  typedef item_vector_t::value_type_ptr value_type_ptr;
  typedef ItemPtrVector self;
  typedef oos::object_ref<self> self_ref;
  typedef item_vector_t::iterator iterator;
  typedef item_vector_t::const_iterator const_iterator;

public:
  ItemPtrVector()
    : item_vector_(this, "itemlist", "itemindex")
  {}
  virtual ~ItemPtrVector() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_vector("item_vector", item_vector_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object_vector("item_vector", item_vector_);
  }

  void push_front(const value_type_ptr &i)
  {
    item_vector_.push_front(i);
  }

  void push_back(const value_type_ptr &i)
  {
    item_vector_.push_back(i);
  }

  iterator begin() { return item_vector_.begin(); }
  const_iterator begin() const { return item_vector_.begin(); }

  iterator end() { return item_vector_.end(); }
  const_iterator end() const { return item_vector_.end(); }

  bool empty() const { return item_vector_.empty(); }
  void clear() { item_vector_.clear(); }

  iterator erase(iterator i)
  {
    return item_vector_.erase(i);
  }

  size_t size() { return item_vector_.size(); }

private:
  item_vector_t item_vector_;
};

#endif /* ITEM_HPP */
