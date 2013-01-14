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

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_container.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_node.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <vector>
#include <iostream>

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
  typedef typename base_item::size_type size_type;

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

  virtual void deserialize(object_reader &deserializer)
  {
    base_item::deserialize(deserializer);
    deserializer.read("item_index", index_);
  }
  virtual void serialize(object_writer &serializer) const
  {
    base_item::serialize(serializer);
    serializer.write("item_index", index_);
  }

  size_type index() const
  {
    return index_;
  }

  void index(size_type i)
  {
    base_item::modify(index_, i);
  }

private:
  size_type index_;
};

/// @endcond

/**
 * @class object_vector_base
 * @brief Base class for all object vector classes.
 * @tparam S The type of the parent object.
 * @tparam T The value of the vector.
 * @tparam CT The container item type.
 * 
 * The object_vector class implements a vector which
 * can hold any type of object from builtin types as
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
  typedef CT item_holder;                                    /**< Shortcut for the value holder type. */
  typedef typename CT::object_type item_type;                /**< Shortcut for the item type. */
  typedef std::vector<item_holder> vector_type;                        /**< Shortcut for the vector class member. */
  typedef typename vector_type::iterator iterator;                  /**< Shortcut for the vector iterator. */
  typedef typename vector_type::const_iterator const_iterator;      /**< Shortcut for the vector const iterator. */
  typedef typename object_container::size_type size_type;           /**< Shortcut for the size type. */

  /**
   * @brief Creates a new object_vector.
   * 
   * A new object_vector is created. The vector is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given vector_name. Also the index
   * of the element is stored within the element. Therefor
   * the name of the index parameter must be given.
   * 
   * @tparam T Parent object type.
   * @param parent The containing vector object.
   */
  object_vector_base(parent_type *parent)
    : parent_(parent)
  {}

  virtual ~object_vector_base() {}

  /**
   * Return the typename of the item type class
   * 
   * @return The item type name.
   */
  virtual const char* classname() const
  {
    return typeid(item_type).name();
  }

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
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const
  {
    const_iterator first = object_vector_.begin();
    const_iterator last = object_vector_.end();
    while (first != last) {
      nf((*first++).ptr());
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
   * Sets the parent for the vector
   *
   * @param p The parent object of the vector.
   */
  virtual void parent(object *p)
  {
    parent_type *temp = dynamic_cast<parent_type*>(p);
    if (!temp) {
      throw object_exception("couldn't cast object to concrete type");
    }
    parent_ = temp;
  }

  /**
   * Return the parent of the 
   * vector.
   * @return The parent.
   */
  parent_type* parent()
  {
    return parent_;
  }

  /**
   * Return a reference to the
   * underlying vector class object.
   *
   * @return The vector object.
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

private:
  parent_type *parent_;
  vector_type object_vector_;
};

///@cond OOS_DEV

struct dummyy { struct inner {}; typedef inner object_type; };

template < class S, class T,
void (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC1)(const object_ref<S>&) = nullptr,
void (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC2)(int) = nullptr,
int (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::* ...FUNC3)() const
>
class object_vector;

///@endcond

/**
 * @brief Object vector class without relation table
 *
 * @tparam S The type of the parent object.
 * @tparam T The value of the vector.
 * @tparam FUNC1 The parent setter function
 * @tparam FUNC2 The index setter function
 * @tparam FUNC3 The index getter function
 * 
 * The object_vector class implements a vector which
 * can hold any type of object from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T,
void (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC1)(const object_ref<S>&),
void (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC2)(int),
int (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC3)() const
>
class object_vector<S, T, FUNC1, FUNC2, FUNC3> : public object_vector_base<S, T>
{
public:
  typedef object_vector_base<S, T> base_vector;                /**< Shortcut for the base vector. */
	typedef typename T::object_type value_type;                  /**< Shortcut for the value type. */
  typedef T value_holder;                                      /**< Shortcut for the value holder. */
  typedef object_ref<S> parent_ref;                            /**< Shortcut for the parent reference. */
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
   * @param parent The parent object.
   */
	explicit object_vector(S *parent)
    : object_vector_base<S, T>(parent)
		, ref_setter(FUNC1)
		, int_setter(FUNC2)
    , int_getter(FUNC3)
  {}

  virtual ~object_vector() {}

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // determine index
      iterator last = this->vector().end();
      // mark list object as modified
      this->mark_modified(this->parent());
      ref_setter(*x.get(), parent_ref(this->parent()));
      // insert new item object
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
      // mark parent object as modified
      this->mark_modified(this->parent());
      // mark item object as modified
      this->mark_modified((*i).get());
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
    // mark parent object as modified
    this->mark_modified(this->parent());
    iterator i = first;
    while (i != last) {
      // mark item object as modified
      this->mark_modified((*i).get());
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
    return 0;
  }

  virtual void adjust_index(iterator i)
  {
    size_t start = i - this->begin();
    
    while (i != this->vector().end()) {
      // mark item object as modified
      this->mark_modified(i->get());
      int_setter(*(*i++).get(), start++);
    }
  }

  virtual void append_proxy(object_proxy *proxy)
  {    
    int index = int_getter(*static_cast<value_type*>(proxy->obj));
    if (this->vector().size() < (size_type)index) {
      this->vector().resize(index);
      this->vector().push_back(value_holder(proxy));
    } else if (this->vector().size() == (size_type)index) {
      this->vector().push_back(value_holder(proxy));
    } else {
      this->vector()[index] = value_holder(proxy);
    }
  }
///@endcond

private:
	std::function<void (value_type&, const object_ref<S>&)> ref_setter;
	std::function<void (value_type&, int)> int_setter;
	std::function<int (const value_type&)> int_getter;
};

/**
 * @brief Object vector class with relation table
 *
 * @tparam S The type of the parent object.
 * @tparam T The value of the vector.
 * @tparam FUNC1 The parent setter function
 * @tparam FUNC2 The index setter function
 * 
 * The object_vector class implements a vector which
 * can hold any type of object from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T,
void (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC1)(const object_ref<S>&),
void (std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type::*FUNC2)(int)
>
class object_vector<S, T, FUNC1, FUNC2> : public object_vector_base<S, T, object_ptr<object_vector_item<T, S> > >
{
public:
  typedef object_vector_base<S, T, object_ptr<object_vector_item<T, S> > > base_vector;                                   /**< Shortcut for the base vector. */
	typedef typename std::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummyy>::type::object_type value_type; /**< Shortcut for the value type. */
  typedef T value_holder;                                                                                                 /**< Shortcut for the value holder. */
  typedef object_ref<S> parent_ref;                                                                                       /**< Shortcut for the parent reference. */
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
   * @param parent The parent object.
   */
	explicit object_vector(S *parent)
    : object_vector_base<S, T, object_ptr<object_vector_item<T, S> > >(parent)
		, str_setter(FUNC1)
    , int_setter(FUNC2)
	{}

  virtual ~object_vector() {}
	
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
      item_ptr item = this->ostore()->insert(new item_type(parent_ref(this->parent()), index, x));
      // mark list object as modified
      this->mark_modified(this->parent());
      // insert new item object
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
    // erase object from object store
    item_ptr item = *i;
    this->ostore()->remove(item);
    this->mark_modified(this->parent());
    iterator ret = this->vector().erase(i);
    // update index values of all successor elements
    this->adjust_index(ret);

    return ret;
  }

  virtual iterator erase(iterator first, iterator last)
  {
    iterator i = first;
    while (i != last) {
      // erase object from object store
      item_ptr item = *i++;
      this->mark_modified(this->parent());
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
      // mark parent object as modified
      this->mark_modified(i->get());
      (*i++)->index(start++);
    }
  }

  virtual void append_proxy(object_proxy *proxy)
  {
    item_type *val = static_cast<item_type*>(proxy->obj);
    int index = val->index();
    if (this->vector().size() < (size_type)index) {
      this->vector().resize(index);
      this->vector().push_back(item_holder(proxy));
    } else if (this->vector().size() == (size_type)index) {
      this->vector().push_back(item_holder(proxy));
    } else {
      this->vector()[index] = item_holder(proxy);
    }
  }

///@endcond

private:
	std::function<void (value_type&, const object_ref<S>&)> str_setter;
	std::function<void (value_type&, int)> int_setter;
};

}

#endif /* OBJECT_VECTOR_HPP */
