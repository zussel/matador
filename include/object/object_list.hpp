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

#ifndef OBJECT_LIST_HPP
#define OBJECT_LIST_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"

/*
 *   ObjectList layout example:
 * 
 *   ID | ROOT |PREV | NEXT | data
 *   ---+------+-----+------+------+
 *    1 |   1  |  0  |  2   |    0 |
 *    2 |   1  |  1  |  3   |    7 |
 *    3 |   1  |  2  |  4   |    8 |
 *    4 |   1  |  3  |  5   |    9 |
 *    5 |   1  |  4  |  0   |    0 |
 */
 
namespace oos {

template < class T > class object_list;
template < class T > class object_list_iterator;
template < class T > class const_object_list_iterator;

template < class T >
class object_list_node : public object
{
public:
  object_list_node()
    : root_(NULL)
    , prev_(NULL)
    , next_(NULL)
  {}
  virtual ~object_list_node() {}

	void read_from(object_atomizer *reader)
  {
    object::read_from(reader);
    object_ref<T> optr(root_);
    reader->read_object("root", optr);
    optr.reset(prev_);
    reader->read_object("prev", optr);
    optr.reset(next_);
    reader->read_object("next", optr);
  }
	void write_to(object_atomizer *writer)
  {
    object::write_to(writer);
    writer->write_object("root", object_ref<T>(root_));
    writer->write_object("prev", object_ref<T>(prev_));
    writer->write_object("next", object_ref<T>(next_));
  }

private:
  friend class object_list<T>;
  friend class object_list_iterator<T>;
  friend class const_object_list_iterator<T>;

  T* root_;
  T* prev_;
  T* next_;
};

template < class T >
class object_ptr_list_node : public object_list_node<object_ptr_list_node<T> >
{
public:
  typedef T value_type;

  object_ptr_list_node() {}
  object_ptr_list_node(object_ptr<T> *optr) : object_(optr) {}
  virtual ~object_ptr_list_node() {}

	void read_from(object_atomizer *reader)
  {
    object_list_node<object_ptr_list_node<T> >::read_from(reader);
    reader->read_object("object", object_);
  }
	void write_to(object_atomizer *writer)
  {
    object_list_node<object_ptr_list_node<T> >::write_to(writer);
    writer->write_string("object", object_);
  }

  object_ptr<T> optr() const {
    return object_;
  }

private:
  object_ptr<T> object_;
};

template < class T >
class object_ref_list_node : public object_list_node<object_ref_list_node<T> >
{
public:
  typedef T value_type;

  object_ref_list_node() {}
  object_ref_list_node(object_ref<T> oref) : object_(oref) {}
  virtual ~object_ref_list_node() {}

	void read_from(object_atomizer *reader)
  {
    object_list_node<object_ref_list_node<T> >::read_from(reader);
    reader->read_object("object", object_);
  }
	void write_to(object_atomizer *writer)
  {
    object_list_node<object_ref_list_node<T> >::write_to(writer);
    writer->write_string("object", object_);
  }

  object_ref<T> oref() const {
    return object_;
  }

private:
  object_ref<T> object_;
};

template < class T >
class object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
  typedef object_list_iterator<T> self;	
  typedef T* pointer;
  typedef T value_type;
  typedef value_type& reference ;
  typedef object_list<T> list_type;
  typedef pointer node_type;

  object_list_iterator()
    : node_(NULL)
    , list_(NULL)
  {}
  object_list_iterator(node_type node, list_type *l)
    : node_(node)
    , list_(l)
  {}

  object_list_iterator(const object_list_iterator &x)
    : node_(x.node_)
    , list_(x.list_)
  {}
  object_list_iterator& operator=(const object_list_iterator &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  ~object_list_iterator() {}

  bool operator==(const object_list_iterator &i) const {
    return (node_ == i.node_);
  }

  bool operator!=(const object_list_iterator &i) const {
    return (node_ != i.node_);
  }

  self& operator++() {
    increment();
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }

  self& operator--() {
    decrement();
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

  pointer operator->() const {
    return node_->object_;
  }

  value_type operator*() const {
    return this->optr();
  }

  object_ptr<T> optr() const {
    return node_;
  }

private:
  void increment() {
    if (node_ != list_->last_) {
      node_ = node_->next_;
    }
  }
  void decrement() {
    if (node_ != list_->last_) {
      node_ = node_->prev_;
    }
  }

private:
  friend class const_object_list_iterator<T>;

  node_type node_;
  list_type *list_;
};

template < class T >
class const_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
public:
  typedef const_object_list_iterator<T> self;
  typedef object_ptr<T> value_type;
  typedef T* pointer;
  typedef value_type& reference ;
  typedef const object_list<T> list_type;
  typedef pointer node_type;

  const_object_list_iterator()
    : node_(NULL)
    , list_(NULL)
  {}
  const_object_list_iterator(node_type node, list_type *l)
    : node_(node)
    , list_(l)
  {}

  const_object_list_iterator(const object_list_iterator<T> &x)
    : node_(x.node_)
    , list_(x.list_)
  {}
  const_object_list_iterator& operator=(const object_list_iterator<T> &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }
  const_object_list_iterator& operator=(const const_object_list_iterator &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  ~const_object_list_iterator() {}

  bool operator==(const const_object_list_iterator &i) const {
    return (node_ == i.node_);
  }

  bool operator!=(const const_object_list_iterator &i) const {
    return (node_ != i.node_);
  }

  self& operator++() {
    increment();
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }

  self& operator--() {
    decrement();
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

  pointer operator->() const {
    return node_;
  }

  value_type operator*() const {
    return this->optr();
  }

  object_ptr<T> optr() const {
    return node_;
  }

private:
  void increment() {
    if (node_ != list_->last_) {
      node_ = node_->next_;
    }
  }
  void decrement() {
    if (node_ != list_->last_) {
      node_ = node_->prev_;
    }
  }

private:
  node_type node_;
  list_type *list_;
};

class object_list_base
{
public:
  object_list_base()
    : first_obj_(NULL)
    , last_obj_(NULL)
  {}
	virtual ~object_list_base() {}

	void read_from(object_atomizer *) {}
	void write_to(object_atomizer *) {}

protected:
  friend class object_store;

  object_store* ostore() const { return ostore_; }
  object* first_obj() const { return first_obj_; }
  object* last_obj() const { return last_obj_; }

  void first_obj(object *f) { first_obj_ = f; }
  void last_obj(object *l) { last_obj_ = l; }

private:
  object *first_obj_;
  object *last_obj_;
  object_store *ostore_;
};

template < class T >
class object_list : public object_list_base
{
public:
	typedef T t_list_node;
	typedef object_ptr<t_list_node> object_node_ptr;

  object_list()
    : first_(new T)
    , last_(new T)
  {
    first_obj(first_);
    last_obj(last_);
    first_->root_ = first_;
    first_->next_ = last_;
    last_->root_ = first_;
    last_->prev_ = first_;
  }
	virtual ~object_list() {}

  typedef object_list_iterator<T> iterator;
  typedef const_object_list_iterator<T> const_iterator;

  iterator begin() {
    return ++iterator(first_, this);
  }
  const_iterator begin() const {
    return ++const_iterator(first_, this);
  }
  iterator end() {
    return iterator(last_, this);
  }
  const_iterator end() const {
    return const_iterator(last_, this);
  }
  bool empty() const {
    return first_->next_ == last_->prev_;
  }
  size_t size() const
  {
    return std::distance(begin(), end());
  }
  void push_front(T *elem) {
    object_ptr<T> optr(elem);
    if (ostore()) {
        optr = ostore()->insert(elem);
    }
    optr->next_ = first_->next_;
    first_->next_ = elem;
    optr->prev_ = first_;
    first_->next_ = elem;
  }
  void push_back(T *elem) {
    object_ptr<T> optr(elem);
    if (ostore()) {
        optr = ostore()->insert(elem);
    }
    optr->prev_ = last_->prev_;
    last_->prev_->next_ = elem;
    optr->next_ = last_;
    last_->prev_ = elem;
  }
  iterator erase(iterator i);
  iterator erase(iterator first, iterator last);

private:
  friend class object_store;
  friend class object_list_iterator<T>;
  friend class const_object_list_iterator<T>;

  t_list_node *first_;
	t_list_node *last_;
};

}

#endif /* OBJECT_LIST_HPP */
