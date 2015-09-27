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

#include "object/serializable.hpp"
#include "object/identifier.hpp"
#include "object/object_atomizer.hpp"
#include "object/object_list.hpp"
#include "object/object_vector.hpp"
#include "object/linked_object_list.hpp"

#include "tools/time.hpp"
#include "tools/date.hpp"
#include "tools/varchar.hpp"

class Item : public oos::serializable
{
public:
  Item()
    : id_(0)
    , char_('c')
    , float_(3.1415f)
    , double_(1.1414)
    , short_(-128)
    , int_(-65000)
    , long_(-1280000)
    , unsigned_short_(128)
    , unsigned_int_(65000)
    , unsigned_long_(128000)
    , bool_(true)
    , string_("Welt")
    , varchar_("Erde")
  {
    init();
  }
  explicit Item(const std::string &str)
    : id_(0)
    , char_('c')
    , float_(3.1415f)
    , double_(1.1414)
    , short_(-128)
    , int_(-65000)
    , long_(-1280000)
    , unsigned_short_(128)
    , unsigned_int_(65000)
    , unsigned_long_(128000)
    , bool_(true)
    , string_(str)
    , varchar_("Erde")
  {
    init();
  }
  Item(const std::string &str, int i)
    : id_(0)
    , char_('c')
    , float_(3.1415f)
    , double_(1.1414)
    , short_(-128)
    , int_(i)
    , long_(-1280000)
    , unsigned_short_(128)
    , unsigned_int_(65000)
    , unsigned_long_(128000)
    , bool_(true)
    , string_(str)
    , varchar_("Erde")
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
  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
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
    deserializer.read("val_date", date_);
    deserializer.read("val_time", time_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
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
    serializer.write("val_date", date_);
    serializer.write("val_time", time_);
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
  void set_cstr(const char *x, int size)
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

  oos::identifier<unsigned long> id_;

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

  void ref(const value_ref &r) { ref_ = r; }
  void ptr(const value_ptr &p) { ptr_ = p; }

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
class List : public oos::serializable
{
public:
  typedef T value_type;
  typedef List<T> self;
  typedef oos::object_list<self, T, true> list_t;
  typedef typename list_t::item_type item_type;
/*  typedef typename list_t::item_type item_ptr;*/
  typedef typename list_t::size_type size_type;
  typedef typename list_t::iterator iterator;
  typedef typename list_t::const_iterator const_iterator;

public:
  List() {}
  List(const std::string &relation_name)
    : relation_name_(relation_name)
  {}
  virtual ~List() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read(relation_name_.c_str(), list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
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

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read(relation_name_.c_str(), item_list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
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

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read(relation_name_.c_str(), vector_);
  }
  virtual void serialize(oos::object_writer &serializer) const
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

class book : public oos::serializable
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
  virtual ~book() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("title", title_);
    deserializer.read("isbn", isbn_);
    deserializer.read("author", author_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("title", title_);
    serializer.write("isbn", isbn_);
    serializer.write("author", author_);
  }

  std::string title() const { return title_; }
  std::string isbn() const { return isbn_; }
  std::string author() const { return author_; }
};

class book_list : public oos::serializable
{
public:
  typedef oos::object_ref<book> book_ref;
  typedef oos::object_list<book_list, book_ref, true> book_list_t;
  typedef book_list_t::item_ptr item_ptr;
  typedef book_list_t::size_type size_type;
  typedef book_list_t::iterator iterator;
  typedef book_list_t::const_iterator const_iterator;
  
  book_list() {}
  virtual ~book_list() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("book_list", book_list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
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
  oos::identifier<unsigned long> id_;
  book_list_t book_list_;
};

class person : public oos::serializable
{
private:
  oos::identifier<unsigned long> id_;
  std::string name_;

public:
  person() {}
  person(const std::string &name)
    : name_(name)
  {}
  
  virtual ~person() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("name", name_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("name", name_);
  }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }
  
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
//  department() {}
  department() : emp_list_(&employee::dep) {}
  department(const std::string &name)
    : name_(name)
    , emp_list_(&employee::dep)
  {}
  
  virtual ~department() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("name", name_);
    deserializer.read("employee_department", emp_list_);
  }
  virtual void serialize(oos::object_writer &serializer) const
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

class course;

class student : public person
{
public:
  typedef oos::object_ref<course> course_ref;
  typedef oos::object_list<student, course_ref, false> course_list_t;

  virtual void deserialize(oos::object_reader &deserializer)
  {
    person::deserialize(deserializer);
    deserializer.read("students", courses);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    person::serialize(serializer);
    serializer.write("courses", courses);
  }

  course_list_t courses;
};

class course : public oos::serializable
{
public:
  typedef oos::object_ref<student> student_ref;
  typedef oos::object_list<course, student_ref, false> student_list_t;

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    // Todo: handle many to man relation (create table, link serializable)
    oos::read_many_to_many(deserializer, "student_course", "student_id", students);
    deserializer.read("students", students);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("students", students);
  }

  oos::identifier<unsigned long> id_;
  std::string name;
  student_list_t students;
};

class album;

class track : public oos::serializable
{
public:
  typedef oos::object_ref<album> album_ref;

private:
  oos::identifier<unsigned long> id_;
  std::string title_;
  album_ref album_;
  int index_;

public:
  track() : index_(0) {}
  track(const std::string &title)
    : title_(title)
    , index_(0)
  {}
  
  virtual ~track() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("title", title_);
    deserializer.read("album", album_);
    deserializer.read("track_index", index_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("title", title_);
    serializer.write("album", album_);
    serializer.write("track_index", index_);
  }

  unsigned long id() { return id_.value(); }

  std::string title() const { return title_; }
  void title(const std::string &t) { title_ = t; }
  
  album_ref alb() const { return album_; }
  void alb(const album_ref &a) { album_ = a; }
  
  int index() const { return index_; }
  void index(int i) { index_ = i; }
};

class album : public oos::serializable
{
public:
  typedef oos::object_ref<track> track_ref;
  typedef oos::object_vector<album, track_ref, false> track_list_t;
  typedef /*typename*/ track_list_t::size_type size_type;
  typedef track_list_t::iterator iterator;
  typedef track_list_t::const_iterator const_iterator;

private:
  oos::identifier<unsigned long> id_;
  std::string name_;
  track_list_t tracks_;
  
public:
  album()
    : tracks_(&track::alb, &track::index, &track::index)
  {}
  album(const std::string &name)
    : name_(name)
    , tracks_(&track::alb, &track::index, &track::index)
  {}
  
  virtual ~album() {}

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("name", name_);
    deserializer.read("tracks", tracks_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("name", name_);
    serializer.write("tracks", tracks_);
  }

  unsigned long id() { return id_.value(); }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const track_ref &b)
  {
    tracks_.push_back(b);
  }

  iterator insert(iterator pos, const track_ref &b)
  {
    return tracks_.insert(pos, b);
  }

  iterator begin() { return tracks_.begin(); }
  const_iterator begin() const { return tracks_.begin(); }

  iterator end() { return tracks_.end(); }
  const_iterator end() const { return tracks_.end(); }

  iterator erase(iterator i) { return tracks_.erase(i); }
  iterator erase(iterator a, iterator b) { return tracks_.erase(a, b); }

  size_type size() const { return tracks_.size(); }
  bool empty() const { return tracks_.empty(); }
};

class playlist : public oos::serializable
{
public:
  typedef oos::object_ref<track> track_ref;
  typedef oos::object_vector<playlist, track_ref, true> track_list_t;
  typedef /*typename*/ track_list_t::size_type size_type;
  typedef track_list_t::iterator iterator;
  typedef track_list_t::const_iterator const_iterator;

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

  virtual void deserialize(oos::object_reader &deserializer)
  {
    deserializer.read("id", id_);
    deserializer.read("name", name_);
    deserializer.read("tracks", tracks_);
    deserializer.read("backup_tracks", backup_tracks_);
  }
  virtual void serialize(oos::object_writer &serializer) const
  {
    serializer.write("id", id_);
    serializer.write("name", name_);
    serializer.write("tracks", tracks_);
    serializer.write("backup_tracks", backup_tracks_);
  }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const track_ref &b)
  {
    tracks_.push_back(b);
  }

  iterator begin() { return tracks_.begin(); }
  const_iterator begin() const { return tracks_.begin(); }

  iterator end() { return tracks_.end(); }
  const_iterator end() const { return tracks_.end(); }

  iterator erase(iterator i) { return tracks_.erase(i); }

  size_type size() const { return tracks_.size(); }
  bool empty() const { return tracks_.empty(); }
};

class child : public oos::serializable
{
public:
    child() {}
    child(const std::string &n) : name(n) {}
    virtual ~child() {}

    virtual void deserialize(oos::object_reader &r)
    {
        r.read("id", id);
        r.read("name", name);
    }

    virtual void serialize(oos::object_writer &w) const
    {
        w.write("id", id);
        w.write("name", name);
    }

    oos::identifier<unsigned long> id;
    std::string name;
};

class master : public oos::serializable
{
public:
    master() {}
    master(const std::string &n) : name(n) {}
    virtual ~master() {}

    virtual void deserialize(oos::object_reader &r)
    {
        r.read("id", id);
        r.read("name", name);
        r.read("child", children);
    }

    virtual void serialize(oos::object_writer &w) const
    {
        w.write("id", id);
        w.write("name", name);
        w.write("child", children);
    }

    oos::identifier<unsigned long> id;
    std::string name;
    oos::object_ptr<child> children;
};

class children_list : public oos::serializable
{
public:
    typedef oos::object_ref<child> child_ref;
    typedef oos::object_list<children_list, child_ref, true> children_list_t;

    children_list() {}
    children_list(const std::string &n) : name(n) {}
    virtual ~children_list() {}

    virtual void deserialize(oos::object_reader &r)
    {
        r.read("id", id);
        r.read("name", name);
        r.read("children", children);
    }

    virtual void serialize(oos::object_writer &w) const
    {
        w.write("id", id);
        w.write("name", name);
        w.write("children", children);
    }

    oos::identifier<unsigned long> id;
    std::string name;
    children_list_t children;
};

#endif /* ITEM_HPP */
