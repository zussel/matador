#ifndef ITEM_HPP
#define ITEM_HPP

#include "object/object.hpp"
#include "object/object_atomizer.hpp"
#include "object/object_list.hpp"
#include "object/object_vector.hpp"
#include "object/linked_object_list.hpp"

#include "tools/varchar.hpp"

class Item : public oos::object
{
public:
  Item() { init(); }
  explicit Item(const std::string &str)
    : string_(str)
  {
    init();
  }
  Item(const std::string &str, int i)
    : int_(i)
    , string_(str)
  {
    init();
  }
  virtual ~Item() {}

private:
  void init() { cstr_[0] = '\0'; }

public:
  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("val_char", char_);
    deserializer.read("val_float", float_);
    deserializer.read("val_double", double_);
    deserializer.read("val_short", short_);
    deserializer.read("val_int", int_);
    deserializer.read("val_long", long_);
    deserializer.read("val_unsigned_short", unsigned_short_);
    deserializer.read("val_unsigned_int", unsigned_int_);
    deserializer.read("val_unsigned_long", unsigned_long_);
    deserializer.read("val_bool", bool_);
    deserializer.read("val_cstr", cstr_, CSTR_LEN);
    deserializer.read("val_string", string_);
    deserializer.read("val_varchar", varchar_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("val_char", char_);
    serializer.write("val_float", float_);
    serializer.write("val_double", double_);
    serializer.write("val_short", short_);
    serializer.write("val_int", int_);
    serializer.write("val_long", long_);
    serializer.write("val_unsigned_short", unsigned_short_);
    serializer.write("val_unsigned_int", unsigned_int_);
    serializer.write("val_unsigned_long", unsigned_long_);
    serializer.write("val_bool", bool_);
    serializer.write("val_cstr", cstr_, CSTR_LEN);
    serializer.write("val_string", string_);
    serializer.write("val_varchar", varchar_);
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

  virtual void deserialize(oos::object_reader &deserializer)
  {
    Item::deserialize(deserializer);
    deserializer.read("ref", ref_);
    deserializer.read("ptr", ptr_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    Item::serialize(serializer);
    serializer.write("ref", ref_);
    serializer.write("ptr", ptr_);
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
  typedef List<T> self;
  typedef oos::object_list<self, T> list_t;
  typedef typename list_t::item_type item_type;
/*  typedef typename list_t::item_type item_ptr;*/
  typedef typename list_t::size_type size_type;
  typedef typename list_t::iterator iterator;
  typedef typename list_t::const_iterator const_iterator;

public:
  List()
    : list_(this)
  {}
  virtual ~List() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("list", list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("list", list_);
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
//typedef List<int> IntList;

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

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("item_list", item_list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("item_list", item_list_);
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

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("item_vector", vector_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("item_vector", vector_);
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
  std::string author_;

public:
  book() {}
  book(const std::string &title, const std::string &isbn, const std::string &author)
    : title_(title)
    , isbn_(isbn)
    , author_(author)
  {}
  virtual ~book() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("title", title_);
    deserializer.read("isbn", isbn_);
    deserializer.read("author", author_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("title", title_);
    serializer.write("isbn", isbn_);
    serializer.write("author", author_);
  }

  std::string title() const { return title_; }
  std::string isbn() const { return isbn_; }
  std::string author() const { return author_; }
};

class book_list : public oos::object
{
public:
  typedef oos::object_ref<book> book_ref;
  typedef oos::object_list<book_list, book_ref> book_list_t;
  typedef book_list_t::item_ptr item_ptr;
  typedef book_list_t::size_type size_type;
  typedef book_list_t::iterator iterator;
  typedef book_list_t::const_iterator const_iterator;
  
  book_list()
    : book_list_(this)
  {}
  virtual ~book_list() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("book_list", book_list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("book_list", book_list_);
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

class person : public oos::object
{
private:
  std::string name_;

public:
  person() {}
  person(const std::string &name)
    : name_(name)
  {}
  
  virtual ~person() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("name", name_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("name", name_);
  }

  std::string name() const { return name_; }
  void name(const std::string &name) { modify(name_, name); }
  
};

class department;

class employee : public person
{
public:
  typedef oos::object_ref<department> dep_ref;

private:
  dep_ref dep_;
  
public:
  employee() {}
  employee(const std::string &name_) : person(name_) {}
  employee(const std::string &name_, const dep_ref &dep)
    : person(name_)
    , dep_(dep)
  {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    person::deserialize(deserializer);
    deserializer.read("department", dep_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    person::serialize(serializer);
    serializer.write("department", dep_);
  }

  dep_ref dep() const { return dep_; }
  void dep(const dep_ref &d) { dep_ = d; }
};

class department : public oos::object
{
public:
  typedef oos::object_ref<employee> emp_ref;
  typedef oos::object_list<department, emp_ref, &employee::dep> emp_list_t;
  typedef emp_list_t::size_type size_type;
  typedef emp_list_t::iterator iterator;
  typedef emp_list_t::const_iterator const_iterator;

private:
  std::string name_;
  emp_list_t emp_list_;
  
public:
  department() : emp_list_(this) {}
  department(const std::string &name)
    : name_(name)
    , emp_list_(this)
  {}
  
  virtual ~department() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("name", name_);
    deserializer.read("employees", emp_list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("name", name_);
    serializer.write("employees", emp_list_);
  }

  std::string name() const { return name_; }
  void name(const std::string &name) { modify(name_, name); }

  void add(const emp_ref &b)
  {
    // TODO: make it work!
    //b->dep(this);
    emp_list_.push_back(b);
  }

  iterator begin() { return emp_list_.begin(); }
  const_iterator begin() const { return emp_list_.begin(); }

  iterator end() { return emp_list_.end(); }
  const_iterator end() const { return emp_list_.end(); }

  iterator erase(iterator i) { return emp_list_.erase(i); }

  size_type size() const { return emp_list_.size(); }
  bool empty() const { return emp_list_.empty(); }
};

#endif /* ITEM_HPP */
