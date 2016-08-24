/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ITEM_HPP
#define ITEM_HPP

#include "tools/base_class.hpp"
#include "object/object_ptr.hpp"
#include "object/has_one.hpp"
#include "object/has_many.hpp"

#include "tools/time.hpp"
#include "tools/date.hpp"
#include "tools/identifier.hpp"

#include "tools/varchar.hpp"
#include <ostream>

class Item
{
public:
  Item()
  {
    init();
  }
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
  void init() {
    memset(cstr_, 0, CSTR_LEN);
#ifdef _MSC_VER
    strcpy_s(cstr_, CSTR_LEN, "Hallo");
#else
    strcpy(cstr_, "Hallo");
#endif
    cstr_[5] = '\0';
  }

public:
  template < class SERIALIZER > void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("val_char", char_);
    serializer.serialize("val_float", float_);
    serializer.serialize("val_double", double_);
    serializer.serialize("val_short", short_);
    serializer.serialize("val_int", int_);
    serializer.serialize("val_long", long_);
    serializer.serialize("val_unsigned_short", unsigned_short_);
    serializer.serialize("val_unsigned_int", unsigned_int_);
    serializer.serialize("val_unsigned_long", unsigned_long_);
    serializer.serialize("val_bool", bool_);
    serializer.serialize("val_cstr", cstr_, (size_t)CSTR_LEN);
    serializer.serialize("val_string", string_);
    serializer.serialize("val_varchar", varchar_);
    serializer.serialize("val_date", date_);
    serializer.serialize("val_time", time_);
  }

  unsigned long id() const { return id_.value(); }
  void id(unsigned long i) { id_.value(i); }

  void set_char(char x) { char_ = x; }
  void set_float(float x) { float_ = x; }
  void set_double(double x) { double_ = x; }
  void set_short(short x) { short_ = x; }
  void set_int(int x) { int_ = x; }
  void set_long(long x) { long_ = x; }
  void set_unsigned_short(unsigned short x) { unsigned_short_ = x; }
  void set_unsigned_int(unsigned int x) { unsigned_int_ = x; }
  void set_unsigned_long(unsigned long x) { unsigned_long_ = x; }
  void set_bool(bool x) { bool_ = x; }
  void set_cstr(const char *x, size_t size)
  {
    if (CSTR_LEN < size) {
      throw std::logic_error("not enough character size");
    }
//    mark_modified();
#ifdef _MSC_VER
    strcpy_s(cstr_, CSTR_LEN, x);
#else
    strcpy(cstr_, x);
#endif
  }
  void set_string(const std::string &x) { string_ = x; }
  void set_varchar(const oos::varchar_base &x) { varchar_ = x.c_str(); }
  void set_date(const oos::date &d) { date_ = d; }
  void set_time(const oos::time &d) { time_ = d; }

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
  oos::date get_date() const { return date_; }
  oos::time get_time() const { return time_; }

  friend std::ostream& operator <<(std::ostream &os, const Item &i)
  {
    os << "Item [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  enum { CSTR_LEN=256 };

  oos::identifier<unsigned long> id_ = 0;

  char char_ = 'c';
  float float_ = 3.1415f;
  double double_ = 1.1414;
  short short_ = -128;
  int int_ = -65000;
  long long_ = -128000;
  unsigned short unsigned_short_ = 128;
  unsigned int unsigned_int_ = 65000;
  unsigned long unsigned_long_ = 128000;
  bool bool_ = true;
  char cstr_[CSTR_LEN];
  std::string string_ = "Welt";
  oos::varchar<64> varchar_ = "Erde";
  oos::date date_;
  oos::time time_;
};

class ItemA : public Item {};
class ItemB : public Item {};
class ItemC : public Item {};

template < class T >
class ObjectItem : public Item
{
public:
  typedef oos::object_ptr<T> value_ptr;

  ObjectItem() {}
  ObjectItem(const std::string &n, int i)
    : Item(n, i)
  {}

  template < class SERIALIZER > void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*oos::base_class<Item>(this));
    serializer.serialize("ref", ref_, oos::cascade_type::NONE);
    serializer.serialize("ptr", ptr_, oos::cascade_type::ALL);
  }

  void ref(const value_ptr &r)
  {
    ref_ = r;
  }
  void ptr(const value_ptr &p) { ptr_ = p; }

  value_ptr ref() const { return ref_; }
  value_ptr ptr() const { return ptr_; }

  template < class I >
  friend std::ostream& operator <<(std::ostream &os, const ObjectItem<I> &i)
  {
    os << "ObjectItem [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  oos::has_one<T> ref_;
  oos::has_one<T> ptr_;
};

class ObjectItemList
{
public:
  typedef oos::has_many<ObjectItem<Item>> object_item_list_t;
  typedef typename object_item_list_t::item_type item_type;
  typedef typename object_item_list_t::iterator iterator;
  typedef typename object_item_list_t::const_iterator const_iterator;

  oos::identifier<unsigned long> id;
  std::string name;
  object_item_list_t items;

  ObjectItemList() {}
  explicit ObjectItemList(const std::string &n) : name(n) {}

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
    //s.serialize(items); // -> should create relation as in next line
    //s.serialize("owner_item",items); // -> should create relation as in next line
    s.serialize("object_item_list", items, "list_id", "object_item_id");
  }

  iterator begin() { return items.begin(); }
  iterator end() { return items.end(); }

  const_iterator begin() const { return items.begin(); }
  const_iterator end() const { return items.end(); }

};

/*
template < class T >
class List : public oos::serializable
{
public:
  typedef T value_type;
  typedef List<T> self;
  typedef oos::object_list<self, T, true> list_t;
  typedef typename list_t::item_type item_type;
  typedef typename list_t::size_type size_type;
  typedef typename list_t::iterator iterator;
  typedef typename list_t::const_iterator const_iterator;

public:
  List() {}
  List(const std::string &relation_name)
    : relation_name_(relation_name)
  {}
  virtual ~List() {}

  virtual void deserialize(oos::deserializer &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read(relation_name_.c_str(), list_);
  }
  virtual void serialize(oos::serializer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write(relation_name_.c_str(), list_);
  }

  void push_front(const value_type &i)
  {
    list_.push_front(i);
  }

  void push_back(const value_type &i)
  {
    list_.push_back(i);
  }

  unsigned long id() { return id_.value(); }

  iterator begin() { return list_.begin(); }
  const_iterator begin() const { return list_.begin(); }

  iterator end() { return list_.end(); }
  const_iterator end() const { return list_.end(); }

  bool empty() const { return list_.empty(); }
  void clear() { list_.clear(); }

  size_type size() { return list_.size(); }

private:
  oos::identifier<unsigned long> id_;
  list_t list_;
  std::string relation_name_;
};

typedef List<oos::object_ptr<Item> > ItemPtrList;
typedef List<oos::object_ref<Item> > ItemRefList;
typedef List<int> IntList;

template<class T>
class list_object_producer : public oos::object_base_producer {
public:
  list_object_producer(const std::string &name) : name_(name) {}
  virtual ~list_object_producer() {}

  virtual oos::object_base_producer* clone() const
  {
    return new list_object_producer(name_);
  }

  virtual oos::serializable* create() const
  {
    return new T(name_);
  }

  virtual const char* classname() const
  {
    return classname_.c_str();
  }

private:
  std::string name_;
  static std::string classname_;
};

template < class T >
std::string list_object_producer<T>::classname_ = typeid(T).name();

template < class T >
class LinkedList : public oos::serializable
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
  LinkedList() {}
  LinkedList(const std::string &relation_name)
    : relation_name_(relation_name)
  {}
  virtual ~LinkedList() {}

  virtual void deserialize(oos::deserializer &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read(relation_name_.c_str(), item_list_);
  }
  virtual void serialize(oos::serializer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write(relation_name_.c_str(), item_list_);
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
  oos::identifier<unsigned long> id_;
  item_list_t item_list_;
  std::string relation_name_;
};

typedef LinkedList<int> LinkedIntList;
typedef LinkedList<oos::object_ptr<Item>> LinkedItemPtrList;
typedef LinkedList<oos::object_ref<Item>> LinkedItemRefList;

//class LinkedIntList : public LinkedList<int>
//{
//public:
//  LinkedIntList() : LinkedList<int>("linked_int_list") {}
//  virtual ~LinkedIntList() {}
//};

//class LinkedItemPtrList : public LinkedList<oos::object_ptr<Item> >
//{
//public:
//  LinkedItemPtrList() : LinkedList<oos::object_ptr<Item> >("linked_ptr_list") {}
//  virtual ~LinkedItemPtrList() {}
//};
//
//class LinkedItemRefList : public LinkedList<oos::object_ref<Item> >
//{
//public:
//  LinkedItemRefList() : LinkedList<oos::object_ref<Item> >("linked_ref_list") {}
//  virtual ~LinkedItemRefList() {}
//};

template < class T >
class Vector : public oos::serializable
{
public:
  typedef T value_type;
  typedef oos::object_vector<Vector<T>, value_type, true> vector_t;
  typedef typename vector_t::item_type item_type;
  typedef typename vector_t::item_ptr item_ptr;
  typedef typename vector_t::size_type size_type;
  typedef typename vector_t::iterator iterator;
  typedef typename vector_t::const_iterator const_iterator;

public:
  Vector() {}
  Vector(const std::string &relation_name)
    : relation_name_(relation_name)
  {}
  virtual ~Vector() {}

  virtual void deserialize(oos::deserializer &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read(relation_name_.c_str(), vector_);
  }
  virtual void serialize(oos::serializer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write(relation_name_.c_str(), vector_);
  }

  unsigned long id() { return id_.value(); }

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
  oos::identifier<unsigned long> id_;
  vector_t vector_;
  std::string relation_name_;
};

template<class T>
class vector_object_producer : public oos::object_base_producer {
public:
  vector_object_producer(const std::string &name) : name_(name) {}
  virtual ~vector_object_producer() {}

  virtual oos::object_base_producer* clone() const
  {
    return new vector_object_producer(name_);
  }

  virtual oos::serializable* create() const
  {
    return new T(name_);
  }

  virtual const char* classname() const
  {
    return classname_.c_str();
  }

private:
  std::string name_;
  static std::string classname_;
};

template < class T >
std::string vector_object_producer<T>::classname_ = typeid(T).name();

typedef Vector<int> IntVector;
typedef Vector<oos::object_ptr<Item> > ItemPtrVector;
typedef Vector<oos::object_ref<Item> > ItemRefVector;
*/
class book
{
private:
  oos::identifier<unsigned long> id_;
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
  ~book() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("title", title_);
    serializer.serialize("isbn", isbn_);
    serializer.serialize("author", author_);
  }

  unsigned long id() const { return id_; }
  void id(unsigned long i)  { id_ = i; }
  std::string title() const { return title_; }
  std::string isbn() const { return isbn_; }
  std::string author() const { return author_; }
};

class book_list
{
public:
  typedef oos::has_many<book> book_list_t;
  typedef book_list_t::size_type size_type;
  typedef book_list_t::iterator iterator;
  typedef book_list_t::const_iterator const_iterator;
  
  book_list() {}
  ~book_list() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("books", book_list_, "book_list_id", "book_id");
  }

  void add(const oos::object_ptr<book> &b)
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
  oos::identifier<unsigned long> id_;
  book_list_t book_list_;
};

class person
{
private:
  oos::identifier<unsigned long> id_;
  oos::varchar<256> name_;
  oos::date birthdate_;
  unsigned int height_ = 0;

public:
  person() {}
  person(unsigned long id, const std::string &name, const oos::date &birthdate, unsigned int height)
    : id_(id)
    , name_(name)
    , birthdate_(birthdate)
    , height_(height)
  {}

  person(const std::string &name, const oos::date &birthdate, unsigned int height)
    : person(0, name, birthdate, height)
  {}

  virtual ~person() {}

  template < class T >
  void serialize(T &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_);
    serializer.serialize("birthdate", birthdate_);
    serializer.serialize("height", height_);
  }

  void id(unsigned long i) { id_.value(i); }
  unsigned long id() const { return id_.value(); }

  std::string name() const { return name_.str(); }
  void name(const std::string &name) { name_ = name; }

  oos::date birthdate() const { return birthdate_; }
  void birthdate(const oos::date &birthdate) { birthdate_ = birthdate; }

  unsigned int height() const { return height_; }
  void height(unsigned int height) { height_ = height; }
};
/*
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

  virtual void deserialize(oos::deserializer &deserializer)
  {
    person::deserialize(deserializer);
    deserializer.read("department", dep_);
  }
  virtual void serialize(oos::serializer &serializer) const
  {
    person::serialize(serializer);
    serializer.write("department", dep_);
  }

  dep_ref dep() const { return dep_; }
  void dep(const dep_ref &d) { dep_ = d; }
};

class department : public oos::serializable
{
public:
  typedef oos::object_ref<employee> emp_ref;
  typedef oos::object_list<department, emp_ref, false> emp_list_t;
  typedef emp_list_t::size_type size_type;
  typedef emp_list_t::iterator iterator;
  typedef emp_list_t::const_iterator const_iterator;

private:
  oos::identifier<unsigned long> id_;
  std::string name_;
  emp_list_t emp_list_;
  
public:
  department()
    : emp_list_(&employee::dep)
  {}
  department(const std::string &name)
    : name_(name)
    , emp_list_(&employee::dep)
  {}
  
  virtual ~department() {}

  virtual void deserialize(oos::deserializer &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("name", name_);
    deserializer.read("employee_department", emp_list_);
  }
  virtual void serialize(oos::serializer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("name", name_);
    serializer.write("employee_department", emp_list_);
  }

  unsigned long id() const { return id_.value(); }
  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const emp_ref &b)
  {
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
*/
class course;

class student : public person
{
public:
  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*oos::base_class<person>(this));
    serializer.serialize("student_course", courses, "student", "course");
  }

  oos::has_many<course, std::list> courses;
};

class course
{
public:

  course() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("title", title);
    serializer.serialize("student_course", students, "student", "course");
  }

  oos::identifier<unsigned long> id;
  std::string title;
  oos::has_many<student, std::list> students;
};

class album;

class track
{
public:
  typedef oos::object_ptr<album> album_ptr;

private:
  oos::identifier<unsigned long> id_;
  std::string title_;
  oos::has_one<album> album_;
  int index_;

public:
  track() : index_(0) {}
  track(const std::string &title)
    : title_(title)
    , index_(0)
  {}
  
  ~track() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("title", title_);
    serializer.serialize("album", album_);
    serializer.serialize("track_index", index_);
  }

  unsigned long id() { return id_.value(); }

  std::string title() const { return title_; }
  void title(const std::string &t) { title_ = t; }

  album_ptr alb() const { return album_; }
  void alb(const album_ptr &a) { album_ = a; }
  
  int index() const { return index_; }
  void index(int i) { index_ = i; }
};

class album
{
public:
  typedef oos::object_ptr<track> track_ptr;
  typedef oos::has_many<track> track_vector_t;
  typedef track_vector_t::size_type size_type;
  typedef track_vector_t::iterator iterator;
//  typedef track_vector_t::const_iterator const_iterator;

private:
  oos::identifier<unsigned long> id_;
  std::string name_;
  track_vector_t tracks_;
  
public:
  album()
//    : tracks_(&track::alb, &track::index, &track::index)
  {}
  album(const std::string &name)
    : name_(name)
//    , tracks_(&track::alb, &track::index, &track::index)
  {}
  
  ~album() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_);
    serializer.serialize("tracks", tracks_);
  }

  unsigned long id() { return id_.value(); }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const track_ptr &t)
  {
    tracks_.push_back(t);
  }

  iterator insert(iterator pos, const track_ptr &b)
  {
    return tracks_.insert(pos, b);
  }

  iterator begin() { return tracks_.begin(); }
//  const_iterator begin() const { return tracks_.begin(); }

  iterator end() { return tracks_.end(); }
//  const_iterator end() const { return tracks_.end(); }

  iterator erase(iterator i) { return tracks_.erase(i); }
  iterator erase(iterator a, iterator b) { return tracks_.erase(a, b); }

  size_type size() const { return tracks_.size(); }
  bool empty() const { return tracks_.empty(); }
};

class playlist
{
public:
  typedef oos::object_ptr<track> track_ref;
  typedef oos::has_many<track> track_list_t;
  typedef track_list_t::size_type size_type;
  typedef track_list_t::iterator iterator;
//  typedef track_list_t::const_iterator const_iterator;

private:
  oos::identifier<unsigned long> id_;
  std::string name_;
  track_list_t tracks_;
  track_list_t backup_tracks_;
  
public:
  playlist() {}
  playlist(const std::string &name)
    : name_(name)
  {}
  
  virtual ~playlist() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_);
    serializer.serialize("playlist_tracks", tracks_);
    serializer.serialize("backup_tracks", backup_tracks_);
  }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const track_ref &b)
  {
    tracks_.push_back(b);
  }

  iterator begin() { return tracks_.begin(); }
//  const_iterator begin() const { return tracks_.begin(); }

  iterator end() { return tracks_.end(); }
//  const_iterator end() const { return tracks_.end(); }

  iterator erase(iterator i) { return tracks_.erase(i); }

  size_type size() const { return tracks_.size(); }
  bool empty() const { return tracks_.empty(); }
};

class child
{
public:
  child() {}
  child(const std::string &n) : name(n) {}
  ~child() {}

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
  }

  oos::identifier<unsigned long> id;
  std::string name;
};

class master
{
public:
  oos::identifier<unsigned long> id;
  std::string name;
  oos::has_one<child> children;

public:
  master() {}
  master(const std::string &n) : name(n) {}
  ~master() {}

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("child", children, oos::cascade_type::DELETE);
  }
};

class children_vector
{
public:
  typedef oos::has_many<child> children_vector_t;

  children_vector() {}
  children_vector(const std::string &n) : name(n) {}
  ~children_vector() {}

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("children", children, "vector_id", "child_id");
  }

  oos::identifier<unsigned long> id;
  std::string name;
  children_vector_t children;
};

class children_list
{
public:
  typedef oos::has_many<child, std::list> children_list_t;

  children_list() {}
  children_list(const std::string &n) : name(n) {}
  ~children_list() {}

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("children", children, "list_id", "child_id");
  }

  oos::identifier<unsigned long> id;
  std::string name;
  children_list_t children;
};

#endif /* ITEM_HPP */
