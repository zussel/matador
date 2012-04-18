#ifndef ITEM_HPP
#define ITEM_HPP

#include "object/object.hpp"
#include "object/object_list.hpp"
#include "object/object_vector.hpp"
#include "object/linked_object_list.hpp"
#include "object/object_atomizer.hpp"

#include "tools/varchar.hpp"

class Item : public oos::object
{
public:
  Item() {}
  explicit Item(const std::string &str)
    : string_(str)
  {}
  Item(const std::string &str, int i)
    : int_(i)
    , string_(str)
  {}
  virtual ~Item() {}

	void read_from(oos::object_atomizer *reader)
  {
    oos::object::read_from(reader);
    reader->read_char("val_char", char_);
    reader->read_float("val_float", float_);
    reader->read_double("val_double", double_);
    reader->read_short("val_short", short_);
    reader->read_int("val_int", int_);
    reader->read_long("val_long", long_);
    reader->read_unsigned_short("val_unsigned_short", unsigned_short_);
    reader->read_unsigned_int("val_unsigned_int", unsigned_int_);
    reader->read_unsigned_long("val_unsigned_long", unsigned_long_);
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
    writer->write_short("val_short", short_);
    writer->write_int("val_int", int_);
    writer->write_long("val_long", long_);
    writer->write_unsigned_short("val_unsigned_short", unsigned_short_);
    writer->write_unsigned_int("val_unsigned_int", unsigned_int_);
    writer->write_unsigned_long("val_unsigned_long", unsigned_long_);
    writer->write_bool("val_bool", bool_);
    writer->write_string("val_string", string_);
    writer->write_varchar("val_varchar", varchar_);
  }

  void set_char(char x) { modify(char_, x); }
  void set_float(float x) { modify(float_, x); }
  void set_double(double x) { modify(double_, x); }
  void set_short(short x) { modify(short_, x); }
  void set_int(int x) { modify(int_, x); }
  void set_long(long x) { modify(long_, x); }
  void set_unsigned_short(unsigned short x) { modify(unsigned_short_, x); }
  void set_unsigned_int(unsigned int x) { modify(unsigned_int_, x); }
  void set_unsigned_long(unsigned long x) { modify(unsigned_long_, x); }
  void set_bool(bool x) { modify(bool_, x); }
  void set_string(const std::string &x) { modify(string_, x); }
  void set_varchar(const oos::varchar_base &x) { modify(varchar_, x); }

  char get_char() const { return char_; }
  float get_float() const { return float_; }
  double get_double() const { return double_; }
  short get_short() const { return short_; }
  int get_int() const { return int_; }
  long get_long() const { return long_; }
  unsigned short get_unsigned_short() const { return unsigned_short_; }
  unsigned int get_unsigned_int() const { return unsigned_int_; }
  unsigned long get_unsigned_long() const { return unsigned_long_; }
  bool get_bool() const { return bool_; }
  std::string get_string() const { return string_; }
  oos::varchar_base get_varchar() const { return varchar_; }

  friend std::ostream& operator <<(std::ostream &os, const Item &i)
  {
    os << "Item [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  char char_;
  float float_;
  double double_;
  short short_;
  int int_;
  long long_;
  unsigned short unsigned_short_;
  unsigned int unsigned_int_;
  unsigned long unsigned_long_;
  bool bool_;
  std::string string_;
  oos::varchar<64> varchar_;
};

class ItemA : public Item {};
class ItemB : public Item {};
class ItemC : public Item {};

template < class T >
class ObjectItem : public Item
{
public:
  typedef oos::object_ptr<T> value_ptr;
  typedef oos::object_ref<T> value_ref;

  ObjectItem() {}
  ObjectItem(const std::string &n, int i)
    : Item(n, i)
  {}
  virtual ~ObjectItem() {}

	void read_from(oos::object_atomizer *reader)
  {
    Item::read_from(reader);
    reader->read_object("ref", ref_);
    reader->read_object("ptr", ptr_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    Item::write_to(writer);
    writer->write_object("ref", ref_);
    writer->write_object("ptr", ptr_);
  }

  void ref(const value_ref &r) { modify(ref_, r); }
  void ptr(const value_ptr &p) { modify(ptr_, p); }

  value_ref ref() const { return ref_; }
  value_ptr ptr() const { return ptr_; }

  template < class I >
  friend std::ostream& operator <<(std::ostream &os, const ObjectItem<I> &i)
  {
    os << "ObjectItem [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  value_ref ref_;
  value_ptr ptr_;
};

template < class C >
class ContainerItem : public Item
{
public:
  typedef oos::object_ref<C> container_ref;

  ContainerItem() {}
  ContainerItem(const std::string &name)
    : Item(name)
    , index_(0)
  {}
  virtual ~ContainerItem() {}

	void read_from(oos::object_atomizer *reader)
  {
    Item::read_from(reader);
    reader->read_int("item_index", index_);
    reader->read_object("container", container_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    Item::write_to(writer);
    writer->write_int("item_index", index_);
    writer->write_object("container", container_);
  }
  
  int index() const { return index_; }
  void index(int i) { modify(index_, i); }

  container_ref container() const { return container_; }
  void container(const container_ref &l) { modify(container_, l); }

private:
  int index_;
  container_ref container_;
};

class ItemPtrList : public oos::object
{
public:
  typedef Item item_type;
  typedef oos::object_ptr<Item> value_type;
  typedef oos::object_list<ItemPtrList, value_type> item_list_t;
  typedef item_list_t::size_type size_type;
  typedef ItemPtrList self;
  typedef oos::object_ref<self> self_ref;
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;

public:
  ItemPtrList()
    : item_list_(this)
  {}
  virtual ~ItemPtrList() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_container("item_list", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object_container("item_list", item_list_);
  }

  void push_front(const value_type &i)
  {
    item_list_.push_front(i);
  }

  void push_back(const value_type &i)
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
  typedef oos::object_ref<Item> value_type;
  typedef oos::object_list<ItemRefList, value_type> item_list_t;
  typedef item_list_t::size_type size_type;
  typedef ItemRefList self;
  typedef oos::object_ref<self> self_ref;
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;

public:
  ItemRefList()
    : item_list_(this)
  {}
  virtual ~ItemRefList() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_container("item_list", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object_container("item_list", item_list_);
  }
  
  void push_front(const value_type &i)
  {
    item_list_.push_front(i);
  }
  void push_back(const value_type &i)
  {
    item_list_.push_back(i);
  }

  iterator begin() { return item_list_.begin(); }
  const_iterator begin() const { return item_list_.begin(); }

  iterator end() { return item_list_.end(); }
  const_iterator end() const { return item_list_.end(); }

  bool empty() const { return item_list_.empty(); }
  void clear() { item_list_.clear(); }
  
  size_type size() const { return item_list_.size(); }

private:
  item_list_t item_list_;
};

/*
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
*/
class LinkedItemList : public oos::object
{
  typedef oos::object_ptr<Item> value_type;
  typedef oos::linked_object_list<LinkedItemList, value_type> item_list_t;
  typedef LinkedItemList self;
  typedef oos::object_ref<self> self_ref;

public:
  typedef item_list_t::iterator iterator;
  typedef item_list_t::const_iterator const_iterator;
  typedef item_list_t::size_type size_type;

public:
  LinkedItemList()
    : item_list_(this)
  {}
  virtual ~LinkedItemList() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_container("item_list", item_list_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object_container("item_list", item_list_);
  }
  
  void push_front(const value_type &i)
  {
    item_list_.push_front(i);
  }

  void push_back(const value_type &i)
  {
    item_list_.push_back(i);
  }

  iterator begin() { return item_list_.begin(); }
  const_iterator begin() const { return item_list_.begin(); }

  iterator end() { return item_list_.end(); }
  const_iterator end() const { return item_list_.end(); }

  bool empty() const { return item_list_.empty(); }
  void clear() { item_list_.clear(); }

  size_type size() const { return item_list_.size(); }

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
  typedef oos::object_ptr<Item> value_type;
  typedef oos::object_vector<ItemPtrVector, value_type> item_vector_t;
  typedef item_vector_t::size_type size_type;
  typedef ItemPtrVector self;
  typedef oos::object_ref<self> self_ref;
  typedef item_vector_t::iterator iterator;
  typedef item_vector_t::const_iterator const_iterator;

public:
  ItemPtrVector()
    : item_vector_(this)
  {}
  virtual ~ItemPtrVector() {}

	void read_from(oos::object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object_container("item_vector", item_vector_);
  }
	void write_to(oos::object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object_container("item_vector", item_vector_);
  }

  void push_back(const value_type &i)
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

  size_type size() { return item_vector_.size(); }

private:
  item_vector_t item_vector_;
};

class book : public oos::object
{
private:
  std::string title_;
  std::string isbn_;

public:
  book() {}
  book(const std::string &title, const std::string &isbn)
    : title_(title)
    , isbn_(isbn)
  {}
  virtual ~book() {}
  
  virtual void read_from(oos::object_atomizer *oa)
  {
    oos::object::read_from(oa);
    oa->read_string("title", title_);
    oa->read_string("isbn", isbn_);
  }
  virtual void write_to(oos::object_atomizer *oa) const
  {
    oos::object::write_to(oa);
    oa->write_string("title", title_);
    oa->write_string("isbn", isbn_);
  }
  
  std::string title() const { return title_; }
  std::string isbn() const { return isbn_; }
};

class book_list : public oos::object
{
public:
  typedef oos::object_list<book_list, oos::object_ref<book> > book_list_t;
  typedef book_list_t::item_type item_type;
  typedef book_list_t::item_ptr item_ptr;
  typedef book_list_t::size_type size_type;
  typedef book_list_t::iterator iterator;
  typedef book_list_t::const_iterator const_iterator;
  
  book_list()
    : book_list_(this)
  {}
  virtual ~book_list() {}

  virtual void read_from(oos::object_atomizer *oa)
  {
    oos::object::read_from(oa);
    oa->read_object_container("book_list", book_list_);
  }
  virtual void write_to(oos::object_atomizer *oa) const
  {
    oos::object::write_to(oa);
    oa->write_object_container("book_list", book_list_);
  }

  void add(const oos::object_ref<book> &b)
  {
    book_list_.push_back(b);
  }

  iterator begin() { return book_list_.begin(); }
  const_iterator begin() const { return book_list_.begin(); }

  iterator end() { return book_list_.end(); }
  const_iterator end() const { return book_list_.end(); }

  iterator erase(iterator i) { return book_list_.erase(i); }

  size_type size() const { return book_list_.size(); }
  bool empty() const { return book_list_.empty(); }
  
private:
  book_list_t book_list_;
};

#endif /* ITEM_HPP */
