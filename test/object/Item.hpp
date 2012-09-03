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

  virtual void read_from(oos::object_reader *reader)
  {
    oos::object::read_from(reader);
    reader->read("val_char", char_);
    reader->read("val_float", float_);
    reader->read("val_double", double_);
    reader->read("val_short", short_);
    reader->read("val_int", int_);
    reader->read("val_long", long_);
    reader->read("val_unsigned_short", unsigned_short_);
    reader->read("val_unsigned_int", unsigned_int_);
    reader->read("val_unsigned_long", unsigned_long_);
    reader->read("val_bool", bool_);
    reader->read("val_cstr", cstr_, CSTR_LEN);
    reader->read("val_string", string_);
    reader->read("val_varchar", varchar_);
  }
	void write_to(oos::object_writer *writer) const
  {
    oos::object::write_to(writer);
    writer->write("val_char", char_);
    writer->write("val_float", float_);
    writer->write("val_double", double_);
    writer->write("val_short", short_);
    writer->write("val_int", int_);
    writer->write("val_long", long_);
    writer->write("val_unsigned_short", unsigned_short_);
    writer->write("val_unsigned_int", unsigned_int_);
    writer->write("val_unsigned_long", unsigned_long_);
    writer->write("val_bool", bool_);
    writer->write("val_cstr", cstr_, CSTR_LEN);
    writer->write("val_string", string_);
    writer->write("val_varchar", varchar_);
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
  void set_cstr(const char *x, int size) { modify(cstr_, CSTR_LEN, x, size); }
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
  const char* get_cstr() const { return cstr_; }
  std::string get_string() const { return string_; }
  oos::varchar_base get_varchar() const { return varchar_; }

  friend std::ostream& operator <<(std::ostream &os, const Item &i)
  {
    os << "Item [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  enum { CSTR_LEN=256 };

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
  char cstr_[CSTR_LEN];
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

	void read_from(oos::object_reader *reader)
  {
    Item::read_from(reader);
    reader->read("ref", ref_);
    reader->read("ptr", ptr_);
  }
	void write_to(oos::object_writer *writer) const
  {
    Item::write_to(writer);
    writer->write("ref", ref_);
    writer->write("ptr", ptr_);
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

template < class T >
class List : public oos::object
{
public:
  typedef T value_type;
  typedef oos::object_list<List<T>, value_type> list_t;
  typedef typename list_t::item_type item_type;
  typedef typename list_t::item_type item_ptr;
  typedef typename list_t::size_type size_type;
  typedef typename list_t::iterator iterator;
  typedef typename list_t::const_iterator const_iterator;

public:
  List()
    : list_(this)
  {}
  virtual ~List() {}

  virtual void read_from(oos::object_reader *reader)
  {
    object::read_from(reader);
    reader->read("list", list_);
  }
  virtual void write_to(oos::object_writer *writer) const
  {
    object::write_to(writer);
    writer->write("list", list_);
  }

  void push_front(const value_type &i)
  {
    list_.push_front(i);
  }

  void push_back(const value_type &i)
  {
    list_.push_back(i);
  }

  iterator begin() { return list_.begin(); }
  const_iterator begin() const { return list_.begin(); }

  iterator end() { return list_.end(); }
  const_iterator end() const { return list_.end(); }

  bool empty() const { return list_.empty(); }
  void clear() { list_.clear(); }

  size_type size() { return list_.size(); }

private:
  list_t list_;
};

typedef List<oos::object_ptr<Item> > ItemPtrList;
typedef List<oos::object_ref<Item> > ItemRefList;
typedef List<int> IntList;

template < class T >
class LinkedList : public oos::object
{
public:
  typedef T value_type;
  typedef oos::linked_object_list<LinkedList<value_type>, value_type> item_list_t;
  typedef typename item_list_t::iterator iterator;
  typedef typename item_list_t::item_type item_type;
  typedef typename item_list_t::item_ptr item_ptr;
  typedef typename item_list_t::const_iterator const_iterator;
  typedef typename item_list_t::size_type size_type;

public:
  LinkedList()
    : item_list_(this)
  {}
  virtual ~LinkedList() {}

  virtual void read_from(oos::object_reader *reader)
  {
    object::read_from(reader);
    reader->read("item_list", item_list_);
  }
  virtual void write_to(oos::object_writer *writer) const
  {
    object::write_to(writer);
    writer->write("item_list", item_list_);
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

typedef LinkedList<oos::object_ptr<Item> > LinkedItemPtrList;
typedef LinkedList<oos::object_ref<Item> > LinkedItemRefList;
typedef LinkedList<int> LinkedIntList;

template < class T >
class Vector : public oos::object
{
public:
  typedef T value_type;
  typedef oos::object_vector<Vector<T>, value_type> vector_t;
  typedef typename vector_t::item_type item_type;
  typedef typename vector_t::item_ptr item_ptr;
  typedef typename vector_t::size_type size_type;
  typedef typename vector_t::iterator iterator;
  typedef typename vector_t::const_iterator const_iterator;

public:
  Vector()
    : vector_(this)
  {}
  virtual ~Vector() {}

  virtual void read_from(oos::object_reader *reader)
  {
    object::read_from(reader);
    reader->read("item_vector", vector_);
  }
  virtual void write_to(oos::object_writer *writer) const
  {
    object::write_to(writer);
    writer->write("item_vector", vector_);
  }

  void push_back(const value_type &i)
  {
    vector_.push_back(i);
  }

  iterator begin() { return vector_.begin(); }
  const_iterator begin() const { return vector_.begin(); }

  iterator end() { return vector_.end(); }
  const_iterator end() const { return vector_.end(); }

  bool empty() const { return vector_.empty(); }
  void clear() { vector_.clear(); }

  iterator erase(iterator i)
  {
    return vector_.erase(i);
  }

  iterator erase(iterator first, iterator last)
  {
    return vector_.erase(first, last);
  }

  size_type size() { return vector_.size(); }

private:
  vector_t vector_;
};

typedef Vector<oos::object_ptr<Item> > ItemPtrVector;
typedef Vector<oos::object_ref<Item> > ItemRefVector;
typedef Vector<int> IntVector;

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
  
  virtual void read_from(oos::object_reader *reader)
  {
    oos::object::read_from(reader);
    reader->read("title", title_);
    reader->read("isbn", isbn_);
  }
  virtual void write_to(oos::object_writer *writer) const
  {
    oos::object::write_to(writer);
    writer->write("title", title_);
    writer->write("isbn", isbn_);
  }
  
  std::string title() const { return title_; }
  std::string isbn() const { return isbn_; }
};

class book_list : public oos::object
{
public:
  typedef oos::object_list<book_list, oos::object_ref<book> > book_list_t;
  typedef book_list_t::value_type book_ref;
  typedef book_list_t::item_type item_type;
  typedef book_list_t::item_ptr item_ptr;
  typedef book_list_t::size_type size_type;
  typedef book_list_t::iterator iterator;
  typedef book_list_t::const_iterator const_iterator;
  
  book_list()
    : book_list_(this)
  {}
  virtual ~book_list() {}

  virtual void read_from(oos::object_reader *reader)
  {
    oos::object::read_from(reader);
    reader->read("book_list", book_list_);
  }
  virtual void write_to(oos::object_writer *writer) const
  {
    oos::object::write_to(writer);
    writer->write("book_list", book_list_);
  }

  void add(const book_ref &b)
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
