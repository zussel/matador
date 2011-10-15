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

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

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

class object_list_base_node : public object
{
public:
  object_list_base_node() {}
  virtual ~object_list_base_node() {}

  virtual object_list_base_node *next_node() const = 0;
  virtual object_list_base_node *prev_node() const = 0;
  virtual object_list_base_node *root_node() const = 0;
};

template < class T >
class object_list_node : public object_list_base_node
{
public:
  object_list_node() {}
  virtual ~object_list_node() {}

	void read_from(object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object("root", root_);
    reader->read_object("prev", prev_);
    reader->read_object("next", next_);
  }
	void write_to(object_atomizer *writer)
  {
    object::write_to(writer);
    writer->write_object("root", root_);
    writer->write_object("prev", prev_);
    writer->write_object("next", next_);
  }

  virtual object_list_base_node *next_node() const
  {
    return next_.get();
  }

  virtual object_list_base_node *prev_node() const
  {
    return prev_.get();
  }

  virtual object_list_base_node *root_node() const
  {
    return root_.get();
  }

  void root(const object_ref<T> &r)
  {
    mark_modified();
    root_ = r;
  }

  object_ref<T> root() const
  {
    return root_;
  }

  void prev(const object_ref<T> &p)
  {
    mark_modified();
    prev_ = p;
  }

  object_ref<T> prev() const
  {
    return prev_;
  }

  void next(const object_ref<T> &n)
  {
    mark_modified();
    next_ = n;
  }

  object_ref<T> next() const
  {
    return next_;
  }

private:
  friend class object_list<T>;
  friend class object_list_iterator<T>;
  friend class const_object_list_iterator<T>;

  object_ref<T> root_;
  object_ref<T> prev_;
  object_ref<T> next_;
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
  typedef object_ptr<T> value_type;
  typedef value_type& reference ;
  typedef object_list<T> list_type;

  object_list_iterator()
    : node_(NULL)
    , list_(NULL)
  {}
  object_list_iterator(value_type node, list_type *l)
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
    return node_.get();
  }

  value_type operator*() const {
    return this->optr();
  }

  object_ptr<T> optr() const {
    return node_;
  }

  object_ref<T> oref() const {
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

  value_type node_;
  list_type *list_;
};

template < class T >
class const_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
public:
  typedef const_object_list_iterator<T> self;
  typedef object_ptr<T> value_type;
  typedef T* pointer;
  typedef value_type& reference ;
  typedef object_list<T> list_type;

  const_object_list_iterator()
    : node_(NULL)
    , list_(NULL)
  {}
  const_object_list_iterator(value_type node, const list_type *l)
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
    return node_.get();
  }

  value_type operator*() const {
    return this->oref();
  }

  object_ref<T> oref() const {
    return node_;
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
  value_type node_;
  const list_type *list_;
};

class OOS_API object_list_base
{
public:
  object_list_base();
	virtual ~object_list_base();

	virtual void read_from(object_atomizer *) = 0;
	virtual void write_to(object_atomizer *) const = 0;

  virtual bool empty() const = 0;
  virtual void clear() = 0;
  virtual size_t size() const = 0;

  object_store* ostore() const { return ostore_; }

protected:
  friend class object_store;
  friend class object_creator;
  friend class object_deleter;

  virtual void install(object_store *ostore);
  virtual void uninstall();

  virtual object_list_base_node* first_object() const = 0;
  virtual object_list_base_node* last_object() const = 0;

  object* parent_object() const { return parent_; }
  void parent_object(object *parent) { parent_ = parent; }

private:
  object_store *ostore_;
  object *parent_;
};

template < class T >
class object_list : public object_list_base
{
public:
	typedef T t_list_node;
	typedef object_ptr<t_list_node> object_node_ptr;

  object_list() {}
	virtual ~object_list() {}

  typedef object_list_iterator<T> iterator;
  typedef const_object_list_iterator<T> const_iterator;

	virtual void read_from(object_atomizer *) {}
	virtual void write_to(object_atomizer *) const {}

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

  virtual bool empty() const {
    return first_->next_ == last_->prev_;
  }
  virtual void clear()
  {
    erase(begin(), end());
  }
  virtual size_t size() const
  {
    return std::distance(begin(), end());
  }

  //void push_front(object_ptr<T> elem)
  virtual void push_front(T *elem)
  {
    if (!ostore()) {
      //throw object_exception();
    } else {
      object_ptr<T> optr = ostore()->insert(elem);
      optr->next_ = first_->next_;
      first_->next_ = optr;
      optr->prev_ = first_;
      first_->next_ = optr;
    }
  }

//  void push_back(object_ptr<T> elem)
  virtual void push_back(T* elem)
  {
    if (!ostore()) {
      //throw object_exception();
    } else {
      object_ptr<T> optr = ostore()->insert(elem);
      optr->prev_ = last_->prev_;
      last_->prev_->next_ = optr;
      optr->next_ = last_;
      last_->prev_ = optr;
    }
  }

  iterator erase(iterator i)
  {
    if (!ostore()) {
      // if list is not in ostore
      // throw exception
      //throw object_exception();
      return i;
    }
    // update predeccessor and successor
    object_node_ptr node = (i++).optr();
    node->prev()->next(node->next());
    node->next()->prev(node->prev());
    // delete node
    if (!ostore()->remove(node)) {
      std::cout << "couldn't remove node\n";
      node->prev()->next(node);
      node->next()->prev(node);
      return iterator(node, this);
    }
    // return i's successor
    return i;
  }

  iterator erase(iterator first, iterator last)
  {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

protected:
  virtual void install(object_store *os)
  {
    object_list_base::install(os);
    
    // create first and last element
    first_ = ostore()->insert(new T);
    last_ = ostore()->insert(new T);
    // link object elements
    first_->root_ = first_;
    first_->next_ = last_;
    last_->root_ = first_;
    last_->prev_ = first_;
  }

  virtual void uninstall()
  {
    object_list_base::uninstall();
    first_.reset();
    last_.reset();
  }

  virtual object_list_base_node* first_object() const
  {
    return first_.get();
  }

  virtual object_list_base_node* last_object() const
  {
    return last_.get();
  }

private:
  friend class object_store;
  friend class object_list_iterator<T>;
  friend class const_object_list_iterator<T>;

  object_node_ptr first_;
  object_node_ptr last_;
};

template < class T >
class object_ptr_list : public object_list<object_ptr_list_node<T> >
{
public:
  typedef object_ptr_list_node<T> value_type;

  void push_front(const object_ptr<T> &optr)
  {
    object_ptr_list_node<T> *node = new object_ptr_list_node<T>(optr);
    push_front(node);
  }
  void push_back(const object_ptr<T> &optr)
  {
    object_ptr_list_node<T> *node = new object_ptr_list_node<T>(optr);
    push_front(node);
  }

private:
  virtual void push_front(value_type *elem)
  {
    object_list<value_type>::push_front(elem);
  }

  virtual void push_back(value_type *elem)
  {
    object_list<value_type>::push_back(elem);
  }
};

template < class T >
class object_ref_list : public object_list<object_ref_list_node<T> >
{
public:
  typedef object_ref_list_node<T> value_type;

  void push_front(const object_ref<T> &oref)
  {
    object_ref_list_node<T> *node = new object_ref_list_node<T>(oref);
    push_front(node);
  }
  void push_back(const object_ref<T> &oref)
  {
    object_ref_list_node<T> *node = new object_ref_list_node<T>(oref);
    push_back(node);
  }

private:
  virtual void push_front(value_type *elem)
  {
    object_list<value_type>::push_front(elem);
  }

  virtual void push_back(value_type *elem)
  {
    object_list<value_type>::push_back(elem);
  }
};

}

#endif /* OBJECT_LIST_HPP */
