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
#include "object/object_value.hpp"
#include "object/object_container.hpp"
#include "object/object_exception.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <vector>

namespace oos {

///@cond OOS_DEV
template < class T, class C >
class object_vector_item : public oos::object
{
public:
  typedef oos::object_ref<C> container_ref;
  typedef T value_type;
  typedef unsigned int size_type;

  object_vector_item() : index_(0) {}
  explicit object_vector_item(const container_ref &c)
    : container_(c)
    , index_(0)
  {}
  object_vector_item(const container_ref &c, const value_type &v)
    : container_(c)
    , index_(0)
    , value_(v)
  {}
  object_vector_item(const container_ref &c, size_type i, const value_type &v)
    : container_(c)
    , index_(i)
    , value_(v)
  {}
  virtual ~object_vector_item() {}

  virtual void read_from(oos::object_atomizer *oa)
  {
    oos::object::read_from(oa);
    oa->read_object("container", container_);
    oa->read_unsigned_int("item_index", index_);
//    read(oa, "value", value_);
  }
  virtual void write_to(oos::object_atomizer *oa) const
  {
    oos::object::write_to(oa);
    oa->write_object("container", container_);
    oa->write_unsigned_int("item_index", index_);
//    write(oa, "value", value_);
  }

  container_ref container() const
  {
    return container_;
  }

  size_type size() const
  {
    return index_;
  }

  value_type value() const
  {
    return value_;
  }

  void value(const value_type &v)
  {
    modify(value_, v);
  }

private:
  container_ref container_;
  size_type index_;
  value_type value_;
};

/// @endcond

/**
 * @cond OOS_DEV
 * @class object_vector
 * @brief An object vector class.
 * @tparam T The concrete object type.
 * @tparam W The object pointer or reference class.
 * 
 * The object_vector class is the base class for
 * The object_ptr_vector class and the object_ref_vector
 * class.
 * It stores a wrapper (object_ptr or object_ref) object
 * around the concrete object type inside a vector.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < typename S, class T >
class object_vector : public object_container
{
public:
  typedef object_vector<S, T> self;                            /**< Shortcut for self. */
  typedef T value_type;                                        /**< Shortcut for the value type. */
  typedef S container_type;
  typedef object_vector_item<value_type, container_type> item_type;
  typedef object_ptr<item_type> item_ptr;
  typedef std::vector<item_ptr> vector_type;         /**< Shortcut for the vector class member. */
  typedef typename vector_type::iterator iterator;             /**< Shortcut for the vector iterator. */
  typedef typename vector_type::const_iterator const_iterator; /**< Shortcut for the vector const iterator. */
  typedef typename vector_type::size_type size_type;           /**< Shortcut for the size type. */

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
  object_vector(S *parent)
    : parent_(parent)
  {}

  virtual ~object_vector() {}

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
  iterator insert(iterator pos, const value_type &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // determine index
      typename item_type::size_type index = 0;
      iterator first = begin();
      iterator last = end();
      if (pos != last && pos != first) {
        --(first = pos);
        index = (*first)->index();
      }
      // create and insert new item
      item_ptr item = ostore()->insert(new item_type(object_ref<container_type>(parent_), index, x));
      // mark list object as modified
      mark_modified(parent_);
      // insert new item object
      pos = object_vector_.insert(pos, item);
      ++(first = pos);
      // adjust indices of successor items
      while (first != last) {
        item = (*first++);
        item->index(++index);
      }
      return pos;
    }
  };

  /**
   * Adds an element to the end of the list.
   *
   * @param x The element to be pushed back.
   */
  void push_back(const value_type &x)
  {
    insert(end(), x);
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  value_type operator[](size_type pos)
  {
    return object_vector_[pos]->value();
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  const value_type operator[](size_type pos) const
  {
    return object_vector_[pos]->value();
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  value_type at(size_type pos)
  {
    return object_vector_.at(pos)->value();
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  const value_type at(size_type pos) const
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
  virtual iterator erase(iterator i)
  {
    iterator ret = object_vector_.erase(i);
    // update index values of all successor elements
    iterator j = ret;
    while (j != object_vector_.end()) {
      size_type index = 0;
      if (!(*j)->get("item_index", index)) {
        // error: throw exception
      } else if (!(*j)->set("item_index", --index)) {
        // error: throw exception
      } else {
        // success
        ++j;
      }
    }
    return ret;
  }

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
  iterator erase(iterator first, iterator last)
  {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

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

private:
  virtual void append_proxy(object_proxy *proxy)
  {
    object_vector_.push_back(value_type(proxy));
  }

private:
  S *parent_;
  vector_type object_vector_;
};

}

#endif /* OBJECT_VECTOR_HPP */
