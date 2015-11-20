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

#ifndef OBJECT_VECTOR_HPP
#define OBJECT_VECTOR_HPP

#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_container.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_node.hpp"

#include "tools/conditional.hpp"

#include <functional>

#include <vector>

namespace oos {

///@cond OOS_DEV

template < class T, class C >
class object_vector_item : public container_item<T, C>
{
private:
  typedef container_item<T, C> base_item;

public:
  typedef typename base_item::container_ref container_ref;
  typedef typename base_item::value_type value_type;
//  typedef typename base_item::size_type size_type;
  typedef std::size_t size_type;

public:
  object_vector_item() : index_(0) {}
  explicit object_vector_item(const container_ref &c)
  : base_item(c)
  , index_(0)
  {}
  object_vector_item(const container_ref &c, const value_type &v)
  : base_item(c, v)
  , index_(0)
  {}
  object_vector_item(const container_ref &c, size_type i, const value_type &v)
  : base_item(c, v)
  , index_(i)
  {}
  virtual ~object_vector_item() {}

  virtual void deserialize(deserializer &deserializer)
  {
    base_item::deserialize(deserializer);
    deserializer.read("item_index", (unsigned long&)index_);
  }
  virtual void serialize(serializer &serializer) const
  {
    base_item::serialize(serializer);
    serializer.write("item_index", (unsigned long)index_);
  }

  size_type index() const
  {
    return index_;
  }

  void index(size_type i)
  {
    index_ =  i;
  }

private:
  size_type index_;
};

/// @endcond

template < class T, class C, class CT >
class object_vector_iterator;

template < class T, class C, class CT >
class const_object_vector_iterator;

/**
 * @class object_vector_base
 * @brief Base class for all serializable vector classes.
 * @tparam S The type of the parent serializable.
 * @tparam T The value of the vector.
 * @tparam CT The container item type.
 * 
 * The object_vector class implements a vector which
 * can hold any type of serializable from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T, class CT = T >
class object_vector_base : public object_container
{
public:
  typedef T value_holder;                                    /**< Shortcut for the value type. */
  typedef S parent_type;                                     /**< Shortcut for the container type. */
  typedef object_ref<S> parent_ref;                          /**< Shortcut for the parent reference. */
  typedef CT item_holder;                                    /**< Shortcut for the value holder type. */
  typedef typename CT::object_type item_type;                /**< Shortcut for the item type. */
  typedef std::vector<item_holder> vector_type;              /**< Shortcut for the vector class member. */
  typedef typename vector_type::iterator iterator;                  /**< Shortcut for the vector iterator. */
  typedef typename vector_type::const_iterator const_iterator;      /**< Shortcut for the vector const iterator. */
  typedef typename object_container::size_type size_type;           /**< Shortcut for the size type. */

//  typedef typename object_vector_iterator<T, S, CT> vector_iterator;
//  typedef typename const_object_vector_iterator<T, S, CT> const_vector_iterator;

  /**
   * @brief Creates a new object_vector.
   *
   * A new object_vector is created. The vector is part
   * of the given parent serializable and therefor a reference
   * to the parent serializable must be found inside the value
   * type serializable with the given vector_name. Also the index
   * of the element is stored within the element. Therefor
   * the name of the index parameter must be given.
   *
   * @tparam T Parent serializable type.
   */
  object_vector_base() {}

  virtual ~object_vector_base() {}

  /**
   * Return the begin iterator of the vector.
   *
   * @return The begin iterator.
   */
  iterator begin() {
    return object_vector_.begin();
  }

  /**
   * Return the begin iterator of the vector.
   *
   * @return The begin iterator.
   */
  const_iterator begin() const {
    return object_vector_.begin();
  }

  /**
   * Return the end iterator of the vector.
   *
   * @return The end iterator.
   */
  iterator end() {
    return object_vector_.end();
  }

  /**
   * Return the end iterator of the vector.
   *
   * @return The end iterator.
   */
  const_iterator end() const {
    return object_vector_.end();
  }

  /**
   * Returns true if the vector is empty.
   *
   * @return True if the vector is empty.
   */
  virtual bool empty() const {
    return object_vector_.empty();
  }

  /**
   * Clears the vector
   */
  virtual void clear()
  {
    erase(begin(), end());
  }

  /**
   * Returns the size of the vector.
   *
   * @return The size of the vector.
   */
  virtual size_type size() const
  {
    return object_vector_.size();
  }

  /**
   * @brief Inserts a new element.
   *
   * Insert a new element at a given iterator position.
   *
   * @param pos The position where to insert.
   * @param x The element to insert.
   * @return The new position iterator.
   */
  virtual iterator insert(iterator pos, const value_holder &x) = 0;

  /**
   * Adds an element to the end of the list.
   *
   * @param x The element to be pushed back.
   */
  void push_back(const value_holder &x)
  {
    insert(end(), x);
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  value_holder operator[](size_type pos)
  {
    return object_vector_[pos]->value();
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  const value_holder operator[](size_type pos) const
  {
    return object_vector_[pos]->value();
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  value_holder at(size_type pos)
  {
    return object_vector_.at(pos)->value();
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  const value_holder at(size_type pos) const
  {
    return object_vector_.at(pos)->value();
  }

  /**
   * @brief Interface to erase an element
   *
   * This is the interface for derived object_list
   * classes to implement their erase element method.
   * Erases a single element from the list.
   *
   * @param i The iterator to erase.
   * @return Returns the next iterator.
   */
  virtual iterator erase(iterator i) = 0;

  /**
   * @brief Erases a range defines by iterators
   *
   * Erase a range of elements from the list. The
   * range is defined by a first and a last iterator.
   *
   * @param first The first iterator of the range to erase.
   * @param last The last iterator of the range to erase.
   * @return Returns the next iterator.
   */
  virtual iterator erase(iterator first, iterator last) = 0;

protected:
  /**
   * @brief Executes the given function serializable for all elements.
   *
   * Executes the given function serializable for all elements.
   *
   * @param pred Function serializable used to be executed on each element.
   */
  virtual void for_each(const proxy_func &pred) const
  {
    const_iterator first = object_vector_.begin();
    const_iterator last = object_vector_.end();
    while (first != last) {
      pred(this->proxy(*first++));
    }
  }

  /**
   * Resets the object_store and clears the vector.
   */
  virtual void uninstall()
  {
    object_container::uninstall();
    object_vector_.clear();
  }

  /**
   * Return a reference to the
   * underlying vector class serializable.
   *
   * @return The vector serializable.
   */
  vector_type& vector()
  {
    return object_vector_;
  }

  /**
   * Adjust indeces of all elements
   * after the given iterator.
   *
   * @param i The start iterator for the index adjustment.
   */
  virtual void adjust_index(iterator i) = 0;

  /**
   * Insert a proxy serializable at a given position.
   *
   * @param index The index to insert at.
   * @param proxy The object_proxy to insert.
   */
  void insert_proxy(size_t index, object_proxy *proxy)
  {
    if (this->vector().size() < (size_type)index) {
      this->vector().resize(index);
      this->vector().push_back(item_holder(proxy));
    } else if (this->vector().size() == (size_type)index) {
      this->vector().push_back(item_holder(proxy));
    } else {
      this->vector()[index] = item_holder(proxy);
    }
  }

private:
  virtual void reset()
  {
    object_vector_.clear();
  }

private:
  vector_type object_vector_;
};

///@cond OOS_DEV

template < class T, class C, class CT >
class object_vector_iterator : public std::iterator<std::random_access_iterator_tag, T> {
public:
  typedef object_vector_iterator<T,C,CT> self;	                /**< Shortcut for this iterator type. */
  typedef object_vector_item<T, C> item_type;
  typedef object_vector_base<C, T, CT> object_vector_type;
  typedef typename object_vector_type::vector_type vector_type;
  typedef typename vector_type::iterator vector_iterator;
  typedef T value_type;                                   /**< Shortcut for value type. */
  typedef value_type* pointer;
  typedef value_type& reference;                          /**< Shortcut for the reference to the value type. */
  typedef typename object_container::size_type size_type; /**< Shortcut for the size type. */

  object_vector_iterator() {}
  ~object_vector_iterator() {}

private:
  object_vector_iterator(const vector_iterator &i)
  : iter_(i) {}

public:

  /**
  * Pre increments the iterator
  *
  * @return Returns iterators successor.
  */
  self& operator++() {
    increment();
    return *this;
  }

  /**
  * Post increments the iterator
  *
  * @return Returns iterator before incrementing.
  */
  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
  * Pre decrements the iterator
  *
  * @return Returns iterators predeccessor.
  */
  self& operator--() {
    decrement();
    return *this;
  }

  /**
  * Post decrements the iterator
  *
  * @return Returns iterator before decrementing.
  */
  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

  /**
  * Returns the pointer to the node.
  *
  * @return The pointer to the node.
  */
  pointer operator->() const {
    return iter_->value();
  }

  /**
  * Returns the node.
  *
  * @return The iterators underlaying node.
  */
  value_type operator*() const {
    return iter_;
  }

private:
  void decrement() { --iter_; }
  void increment() { ++iter_; }

private:
  vector_iterator iter_;
};

struct dummyy { struct inner {}; typedef inner object_type; };

template < class S, class T, bool WITH_JOIN_TABLE >
class object_vector;

///@endcond

#ifdef OOS_DOXYGEN_DOC

// NOTE: Only for doxygen documentation!

/**
 * @brief Object vector class without relation table
 *
 * @tparam S The type of the parent serializable.
 * @tparam T The value of the vector.
 * @tparam WITH_JOIN_TABLE Indicates wether a join serializable/table is used or not..
 * 
 * The object_vector class implements a vector which
 * can hold any type of serializable from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 *
 * S indicates the super class holding the
 * vector and T is the type of the vector item.
 * 
 * The last template argument indicates wether the vector
 * uses a relation serializable/table or not, where true means use
 * a relation serializable/table and false not to.
 * If the value is false the item must be an serializable containing
 * already information about its super/holder serializable.
 */
template < class S, class T, bool WITH_JOIN_TABLE >
class object_vector : public object_vector_base<S, T>
{
public:
  typedef object_ref<S> parent_ref;                            /**< Shortcut for the parent reference. */ // true, false
  typedef void (T::object_type::*FUNC1)(const parent_ref&);    /**< Shortcut for the parent reference setter function. */ // true, false
  typedef void (T::object_type::*FUNC2)(int);                  /**< Shortcut for the index setter function. */ // true, false
  typedef int (T::object_type::*FUNC3)() const;                /**< Shortcut for the index getter function. */ // true

  typedef typename base_vector::size_type size_type;           /**< Shortcut for the size type. */ // true, false
  typedef typename base_vector::iterator iterator;             /**< Shortcut for the iterator. */ // true, false
  typedef typename base_vector::const_iterator const_iterator; /**< Shortcut for the const iterator. */ // true, false

  typedef object_vector_base<S, T> base_vector;                                         /**< Shortcut for the base vector. */ // false
  typedef object_vector_base<S, T, object_ptr<object_vector_item<T, S> > > base_vector; /**< Shortcut for the base vector. */ // true

  typedef typename T::object_type value_type;                  /**< Shortcut for the value type. */ // false
  typedef typename oos::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type value_type; /**< Shortcut for the value type. */ // true
  typedef T value_holder;                                      /**< Shortcut for the value holder. */ // true, false

  typedef typename T::object_type item_type;                   /**< Shortcut for the item type. */ // false
  typedef object_vector_item<T, S> item_type;                  /**< Shortcut for the item type. */ // true
  typedef typename base_vector::item_holder item_holder;       /**< Shortcut for the item holder. */ // true
  typedef item_holder item_ptr;                                /**< Shortcut for the item ptr. */ // true
};

#else

/**
 * @brief Object vector class without relation table
 *
 * @tparam S The type of the parent serializable.
 * @tparam T The value of the vector.
 * @tparam FUNC1 The parent setter function
 * @tparam FUNC2 The index setter function
 * @tparam FUNC3 The index getter function
 * 
 * The object_vector class implements a vector which
 * can hold any type of serializable from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T >
class object_vector<S, T, false> : public object_vector_base<S, T>
{
public:
  typedef void (T::object_type::*FUNC1)(const object_ref<S>&); /**< Shortcut for the parent reference setter function. */
  typedef void (T::object_type::*FUNC2)(int);                  /**< Shortcut for the index setter function. */
  typedef int (T::object_type::*FUNC3)() const;                /**< Shortcut for the index getter function. */
  typedef object_vector_base<S, T> base_vector;                /**< Shortcut for the base vector. */
  typedef typename T::object_type value_type;                  /**< Shortcut for the value type. */
  typedef T value_holder;                                      /**< Shortcut for the value holder. */
  typedef typename base_vector::parent_ref parent_ref;         /**< Shortcut for the parent reference. */
  typedef typename T::object_type item_type;                   /**< Shortcut for the item type. */
  typedef typename base_vector::size_type size_type;           /**< Shortcut for the size type. */
  typedef typename base_vector::iterator iterator;             /**< Shortcut for the iterator. */
  typedef typename base_vector::const_iterator const_iterator; /**< Shortcut for the const iterator. */

public:
  /**
   * @brief Creates an empty vector.
   *
   * Creates an empty vector. The parent reference
   * and the index is holded by the item itself.
   *
   * @param f1 The parent reference setter function.
   * @param f2 The index setter function.
   * @param f3 The index getter function.
   */
  object_vector(FUNC1 f1, FUNC2 f2, FUNC3 f3)
    : ref_setter(std::mem_fn(f1))
    , int_setter(std::mem_fn(f2))
    , int_getter(std::mem_fn(f3))
  {}

  virtual ~object_vector() {}

  /**
   * Return the typename of the item type class
   *
   * @return The item type name.
   */
  virtual const char* classname() const
  {
    return classname_.c_str();
  }

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // mark list serializable as modified
      this->mark_modified(this->owner());
//      this->mark_modified(this->parent());
      ref_setter(*x.get(), parent_ref(this->owner()));
      // insert new item serializable
      pos = this->vector().insert(pos, x);
      iterator first = pos;
      // adjust index
      this->adjust_index(first);
      return pos;
    }
  }

  virtual iterator erase(iterator i)
  {
    if (!this->ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // mark parent serializable as modified
      this->mark_modified(this->owner());
//      this->mark_modified(this->parent());
      // mark item serializable as modified
      this->mark_modified(this->proxy(*i));
//      this->mark_modified((*i).get());
      // set back ref to zero
      ref_setter(*(*i).get(), parent_ref());
      // erase element from list
      i = this->vector().erase(i);
      // update index values of all successor elements
      this->adjust_index(i);
      // return iterator
      return i;
    }
  }

  virtual iterator erase(iterator first, iterator last)
  {
    // mark parent serializable as modified
    this->mark_modified(this->owner());
//    this->mark_modified(this->parent());
    iterator i = first;
    while (i != last) {
      // mark item serializable as modified
      this->mark_modified(this->proxy(*i));
//      this->mark_modified((*i).get());
      // set back ref to zero
      ref_setter(*(*i++).get(), parent_ref());
    }

    i = this->vector().erase(first, last);

    // adjust index
    this->adjust_index(i);

    return i;
  }

protected:
///@cond OOS_DEV

  virtual object_base_producer* create_item_producer() const
  {
//    return new object_producer<item_type>();
    return 0;
  }

  virtual void adjust_index(iterator i)
  {
//    iterator j = this->begin();
    iterator j = this->vector().begin();
    size_t start = static_cast<size_t>(std::distance(j, i));
//    size_t start = i - this->begin();

    while (i != this->vector().end()) {
      // mark item serializable as modified
      this->mark_modified(this->proxy(*i));
//      this->mark_modified(i->get());
      int_setter(*(*i++).get(), start++);
    }
  }

  virtual void append_proxy(object_proxy *proxy)
  {
    size_t index = this->size();
    if (proxy->obj()) {
      index = int_getter(*static_cast<value_type*>(proxy->obj()));
    }
    this->insert_proxy(index, proxy);
  }
///@endcond

private:
  std::function<void (value_type&, const object_ref<S>&)> ref_setter;
  std::function<void (value_type&, size_t)> int_setter;
  std::function<int (const value_type&)> int_getter;

  static std::string classname_;

};

template < class S, class T >
std::string object_vector<S, T, false>::classname_ = typeid(item_type).name();


/**
 * @brief Object vector class with relation table
 *
 * @tparam S The type of the parent serializable.
 * @tparam T The value of the vector.
 * @tparam FUNC1 The parent setter function
 * @tparam FUNC2 The index setter function
 * 
 * The object_vector class implements a vector which
 * can hold any type of serializable from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T >
class object_vector<S, T, true> : public object_vector_base<S, T, object_ptr<object_vector_item<T, S> > >
{
public:
  typedef typename oos::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type value_type; /**< Shortcut for the value type. */
  typedef void (value_type::*FUNC1)(const object_ref<S>&);                                                                /**< Shortcut for the parent reference setter function. */
  typedef void (value_type::*FUNC2)(int);                                                                                 /**< Shortcut for the index setter function. */
  typedef object_vector_base<S, T, object_ptr<object_vector_item<T, S> > > base_vector;                                   /**< Shortcut for the base vector. */
  typedef T value_holder;                                                                                                 /**< Shortcut for the value holder. */
  typedef typename base_vector::parent_ref parent_ref;                                                                    /**< Shortcut for the parent reference. */
  typedef typename base_vector::item_holder item_holder;                                                                  /**< Shortcut for the item holder. */
  typedef object_vector_item<T, S> item_type;                                                                             /**< Shortcut for the item type. */
  typedef item_holder item_ptr;                                                                                           /**< Shortcut for the item ptr. */
  typedef typename base_vector::size_type size_type;                                                                      /**< Shortcut for the size type. */
  typedef typename base_vector::iterator iterator;                                                                        /**< Shortcut for the iterator. */
  typedef typename base_vector::const_iterator const_iterator;                                                            /**< Shortcut for the const iterator. */

public:
  /**
   * @brief Creates an empty vector
   *
   * Creates an empty vector with the
   * given parent. All items are mapped
   * via a relation table
   *
   * @param parent The parent serializable.
   * @param f1 The parent reference setter function.
   * @param f2 The index setter function.
   */
  explicit object_vector(FUNC1 f1 = 0, FUNC2 f2 = 0)
    : str_setter(std::mem_fn(f1))
    , int_setter(std::mem_fn(f2))
  {}

  virtual ~object_vector() {}

  /**
   * Return the typename of the item type class
   *
   * @return The item type name.
   */
  virtual const char* classname() const
  {
    return classname_.c_str();
  }

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // determine index
      iterator last = this->vector().end();
      typename item_type::size_type index = this->vector().size();
      if (index && pos != last) {
        index = (*pos)->index();
      }
      // create and insert new item
      item_ptr item = this->ostore()->insert(new item_type(parent_ref(this->owner()), index, x));
      // mark list serializable as modified
//      this->mark_modified(this->parent());
      this->mark_modified(this->owner());
      // insert new item serializable
      pos = this->vector().insert(pos, item);
      iterator first = pos;
      // adjust indices of successor items
      last = this->vector().end();
      while (++first != last) {
        (*first)->index(++index);
      }
      return pos;
    }
  }

  virtual iterator erase(iterator i)
  {
    // erase serializable from serializable store
    item_ptr item = *i;
    this->mark_modified(this->owner());
//    this->mark_modified(this->parent());
    this->ostore()->remove(item);
    iterator ret = this->vector().erase(i);
    // update index values of all successor elements
    this->adjust_index(ret);

    return ret;
  }

  virtual iterator erase(iterator first, iterator last)
  {
    this->mark_modified(this->owner());
//    this->mark_modified(this->parent());
    iterator i = first;
    while (i != last) {
      // erase serializable from serializable store
      item_ptr item = *i++;
      this->ostore()->remove(item);
    }
    i = this->vector().erase(first, last);
    // adjust index
    this->adjust_index(i);

    return i;
  }

protected:

///@cond OOS_DEV

  virtual object_base_producer* create_item_producer() const
  {
    return new object_producer<item_type>();
  }

  void adjust_index(iterator i)
  {
    size_t start = i - this->begin();

    while (i != this->vector().end()) {
      // mark parent serializable as modified
      this->mark_modified(this->proxy(*i));
//      this->mark_modified(i->get());
      (*i++)->index(start++);
    }
  }

  virtual void append_proxy(object_proxy *proxy)
  {
    size_t index = this->size();
    if (proxy->obj()) {
      index = static_cast<item_type*>(proxy->obj())->index();
    }
    this->insert_proxy(index, proxy);
  }

///@endcond

private:
  std::function<void (value_type&, const object_ref<S>&)> str_setter;
  std::function<void (value_type&, int)> int_setter;
  static std::string classname_;

};

template < class S, class T >
std::string object_vector<S, T, true>::classname_ = typeid(item_type).name();

#endif /* OOS_DOXYGEN_DOC */

}

#endif /* OBJECT_VECTOR_HPP */
