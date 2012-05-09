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
#include "object/object_container.hpp"
#include "object/object_exception.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <list>
#include <algorithm>

namespace oos {

///@cond OOS_DEV

template < class T, class C >
class container_item : public oos::object
{
public:
  typedef oos::object_ref<C> container_ref;
  typedef T value_type;
  typedef unsigned int size_type;

  container_item() {}
  explicit container_item(const container_ref &c)
    : container_(c)
  {}
  container_item(const container_ref &c, const value_type &v)
    : container_(c)
    , value_(v)
  {}
  virtual ~container_item() {}

  virtual void read_from(oos::object_atomizer *oa)
  {
    oos::object::read_from(oa);
    oa->read("container", container_);
    oa->read("value", value_);
  }
  virtual void write_to(oos::object_atomizer *oa) const
  {
    oos::object::write_to(oa);
    oa->write("container", container_);
    oa->write("value", value_);
  }

  container_ref container() const
  {
    return container_;
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
  template < class V, class S > friend class object_list;

  container_ref container_;
  value_type value_;
};

/// @endcond

/**
 * @class object_list
 * @brief An object list class.
 * @tparam S The type of the parent object.
 * @tparam T The value of the list.
 * 
 * The object_list class implements a list which
 * can hold any type of object from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T >
class object_list : public object_container
{
public:
  typedef T value_type;                                         /**< Shortcut for the value type. */
  typedef S container_type;                                     /**< Shortcut for the container type. */
  typedef container_item<value_type, container_type> item_type; /**< Shortcut for the container item. */
  typedef object_ptr<item_type> item_ptr;                       /**< Shortcut for the container item pointer. */
  typedef std::list<item_ptr> list_type;                        /**< Shortcut for the list class member. */
  typedef typename object_container::size_type size_type;       /**< Shortcut for size type. */
  typedef typename list_type::iterator iterator;                /**< Shortcut for the list iterator. */
  typedef typename list_type::const_iterator const_iterator;    /**< Shortcut for the list const iterator. */


  /**
   * Create an empty object list
   * with the given parent object.
   *
   * @param parent The parent object of the list.
   */
  explicit object_list(S *parent)
    : parent_(parent)
  {}

  virtual ~object_list() {}
  
  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  iterator begin() {
    return object_list_.begin();
  }

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  const_iterator begin() const {
    return object_list_.begin();
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  iterator end() {
    return object_list_.end();
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  const_iterator end() const {
    return object_list_.end();
  }

  /**
   * Returns true if the list is empty.
   * 
   * @return True if the list is empty.
   */
  virtual bool empty() const {
    return object_list_.empty();
  }

  /**
   * Clears the list
   */
  virtual void clear()
  {
    erase(begin(), end());
  }

  /**
   * Returns the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_type size() const
  {
    return object_list_.size();
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
      // create and insert new item
      item_ptr item = ostore()->insert(new item_type(object_ref<S>(parent_), x));
      /*
      item->container_.reset(parent_);
      item->value_ = x;
      */
      // mark list object as modified
      mark_modified(parent_);
      // insert new item object
      return object_list_.insert(pos, item);
    }
  };

  /**
   * Adds an element to the beginning of the list.
   *
   * @param x The element to be pushed front.
   */
  void push_front(const value_type &x)
  {
    insert(begin(), x);
  }

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
    if (!ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      item_ptr item = *i;
      mark_modified(parent_);
      if (!ostore()->remove(item)) {
        std::cout << "couldn't remove node (proxy: " << *item->proxy() << ")\n";
        return end();
      } else {
        return object_list_.erase(i);
      }
    }
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
    const_iterator first = object_list_.begin();
    const_iterator last = object_list_.end();
    while (first != last) {
      nf((*first++).ptr());
    }
  }

  /**
   * Resets the object_store and clears the list.
   */
  virtual void uninstall()
  {
    object_container::uninstall();
    object_list_.clear();
  }

  /**
   * Sets the parent for the list
   *
   * @param p The parent object of the list.
   */
  virtual void parent(object *p)
  {
    S *temp = dynamic_cast<S*>(p);
    if (!temp) {
      throw object_exception("couldn't cast object to concrete type");
    }
    parent_ = temp;
  }

private:
  virtual void reset()
  {
    object_list_.clear();
  }

  virtual void append_proxy(object_proxy *proxy)
  {
    object_list_.push_back(item_ptr(proxy));
  }

private:
  list_type object_list_;

  S *parent_;
};

}

#endif /* OBJECT_LIST_HPP */
