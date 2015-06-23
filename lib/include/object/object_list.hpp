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
#include "object/prototype_node.hpp"

#include "tools/conditional.hpp"

#include <functional>

#include <type_traits>
#include <list>
#include <algorithm>

namespace oos {

/**
 * @class object_list_base
 * @brief Base class for all serializable list classes.
 * @tparam S The type of the parent serializable.
 * @tparam T The value of the list.
 * @tparam CT The container item type.
 * 
 * The object_list class implements a list which
 * can hold any type of serializable from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < class S, class T, class CT = T >
class object_list_base : public object_container
{
public:
  typedef T value_holder;                                    /**< Shortcut for the value type. */
  typedef S parent_type;                                     /**< Shortcut for the container type. */
  typedef object_ref<S> parent_ref;                          /**< Shortcut for the parent reference. */
  typedef CT item_holder;                                    /**< Shortcut for the value holder type. */
  typedef typename CT::object_type item_type;                /**< Shortcut for the item type. */
  typedef std::list<item_holder> list_type;                  /**< Shortcut for the list class member. */
  typedef typename object_container::size_type size_type;    /**< Shortcut for size type. */
  typedef typename list_type::iterator iterator;             /**< Shortcut for the list iterator. */
  typedef typename list_type::const_iterator const_iterator; /**< Shortcut for the list const iterator. */

  /**
   * Create an empty serializable list
   * with the given parent serializable.
   *
   * @param parent The parent serializable of the list.
   */
  explicit object_list_base()
  {}

  virtual ~object_list_base() {}
  
  /**
   * Return the class name of the item.
   * 
   * @return The class name of the item.
   */
  virtual const char* classname() const
  {
    return classname_.c_str();
  }

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
  virtual iterator insert(iterator pos, const value_holder &x) = 0;

  /**
   * Adds an element to the beginning of the list.
   *
   * @param x The element to be pushed front.
   */
  void push_front(const value_holder &x)
  {
    insert(begin(), x);
  }

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
  iterator erase(iterator first, iterator last)
  {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

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
    const_iterator first = object_list_.begin();
    const_iterator last = object_list_.end();
    while (first != last) {
      pred(this->proxy(*first++));
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
   * Return the underlying list serializable.
   * 
   * @return The list serializable.
   */
  list_type& list()
  {
    return object_list_;
  }

private:
  virtual void reset()
  {
    object_list_.clear();
  }

  virtual void append_proxy(object_proxy *proxy)
  {
    this->list().push_back(item_holder(proxy));
  }

private:
  list_type object_list_;

  static std::string classname_;
};

template < class S, class T, class CT >
std::string object_list_base<S, T, CT>::classname_ = typeid(parent_ref).name();

///@cond OOS_DEV

/*
 * helper class
 */
struct dummy { struct inner {}; typedef inner object_type; };

/*
 * Not implemented class
 */
template < class S, class T, bool WITH_JOIN_TABLE >
class object_list;

///@endcond

#ifdef OOS_DOXYGEN_DOC

// NOTE: Only for doxygen documentation!

/**
 * @class object_list
 * @brief List class mapping items via relation table
 * 
 * @tparam S The parent class type.
 * @tparam T The item class type.
 * @tparam WITH_JOIN_TABLE Indicates wether a join serializable/table is used or not.
 *
 * The serializable list class represents a list of objects or values
 * in the serializable store. S indicates the super class holding
 * the list and T is the type of the list item.
 * 
 * The last template argument indicates wether the list
 * uses a relation serializable/table or not, where true means use
 * a relation serializable/table and false not to.
 * If the value is false the item must be an serializable containing
 * already information about its super/holder serializable.
 */
template < class S, class T, bool WITH_JOIN_TABLE >
class object_list : public  object_list_base<S, T>
{
public:
  typedef object_ref<S> parent_ref;                           /**< Shortcut for the parent reference. */ // true, false

  typedef void (T::object_type::*SETFUNC)(const parent_ref&); /**< Shortcut for the parent reference setter function. */ // false

  typedef object_list_base<S, T> base_list;                                     /**< Shortcut for the base list. */ // false
  typedef object_list_base<S, T, object_ptr<container_item<T, S> > > base_list; /**< Shortcut for the base list. */ // true

  typedef typename T::object_type item_type;                  /**< Shortcut for the item type. */ // false
  typedef container_item<T, S> item_type;                     /**< Shortcut for the item type. */ // true

  typedef typename base_list::value_holder value_holder;      /**< Shortcut for the value holder. */ // false
  typedef T value_holder;                                     /**< Shortcut for the value holder. */ // true

  typedef typename base_list::size_type size_type;            /**< Shortcut for the size type. */ // true, false
  typedef typename base_list::iterator iterator;              /**< Shortcut for the iterator. */ // true, false
  typedef typename base_list::const_iterator const_iterator;  /**< Shortcut for the const iterator. */ // true, false

  typedef typename base_list::item_holder item_ptr;           /**< Shortcut for the item ptr. */ // true
};

#else
/**
 * @class object_list
 * @brief List class mapping items via relation table
 * 
 * @tparam S The parent class type.
 * @tparam T The item class type.
 * @tparam SETFUNC The parent setter function.
 *
 * This serializable list class uses no relation table. The items
 * hold the information of its parent by themself.
 */
template < class S, class T >
class object_list<S, T, false> : public  object_list_base<S, T>
{
public:
  typedef object_list_base<S, T> base_list;                   /**< Shortcut for the base list. */
  typedef typename T::object_type item_type;                  /**< Shortcut for the item type. */
  typedef typename base_list::parent_ref parent_ref;          /**< Shortcut for the parent reference. */
  typedef void (T::object_type::*SETFUNC)(const parent_ref&); /**< Shortcut for the parent reference setter function. */
  typedef typename base_list::value_holder value_holder;      /**< Shortcut for the value holder. */
  typedef typename base_list::item_holder item_holder;        /**< Shortcut for the item holder. */
  typedef typename base_list::size_type size_type;            /**< Shortcut for the size type. */
  typedef typename base_list::iterator iterator;              /**< Shortcut for the iterator. */
  typedef typename base_list::const_iterator const_iterator;  /**< Shortcut for the const iterator. */

public:
  /**
   * Creates an empty list with the given
   * parent.
   * 
   * @param parent The parent of the list.
   * @param setfunc The parent reference setter function.
   */
  object_list(SETFUNC setfunc)
    : setter_(setfunc)
  {}
  virtual ~object_list() {}

  /**
   * Return the class name of the item.
   * 
   * @return The class name of the item.
   */
//  virtual const char* classname() const
//  {
//    return typeid(parent_ref).name();
//  }

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // mark item serializable as modified
//      this->mark_modified(x.get());
      this->mark_modified(this->proxy(x));
      // set back ref to parent
      setter_(*x.get(), parent_ref(this->owner()));
      // insert new item serializable
      return this->list().insert(pos, x);
    }
  };

  virtual iterator erase(iterator i)
  {
    if (!this->ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // mark item serializable as modified
      this->mark_modified(this->proxy(*i));
//      this->mark_modified((*i).get());
      // set back ref to zero
      setter_(*(*i).get(), parent_ref());
      // erase element from list
      return this->list().erase(i);
    }
  }

protected:
///@cond OOS_DEV
  virtual object_base_producer* create_item_producer() const
  {
    return 0;
  }
///@endcond
private:
  std::function<void (item_type&, const object_ref<S>&)> setter_;

};

/**
 * @class object_list
 * @brief List class mapping items via relation table
 * 
 * @tparam S The parent class type.
 * @tparam T The item class type.
 *
 * This serializable list class uses a relation table to
 * map the items to its parent.
 */
template < class S, class T>
class object_list<S, T, true> : public object_list_base<S, T, object_ptr<container_item<T, S> > >
{
public:
  typedef object_list_base<S, T, object_ptr<container_item<T, S> > > base_list; /**< Shortcut for the base list. */
  typedef T value_holder;                                                       /**< Shortcut for the value holder. */
  typedef container_item<T, S> item_type;                                       /**< Shortcut for the item type. */
  typedef typename base_list::parent_ref parent_ref;                            /**< Shortcut for the parent reference. */
  typedef typename base_list::size_type size_type;                              /**< Shortcut for the size type. */
  typedef typename base_list::item_holder item_holder;                          /**< Shortcut for the item holder. */
  typedef item_holder item_ptr;                                                 /**< Shortcut for the item ptr. */
  typedef typename base_list::iterator iterator;                                /**< Shortcut for the iterator. */
  typedef typename base_list::const_iterator const_iterator;                    /**< Shortcut for the const iterator. */

public:
  /**
   * Creates an empty list with the given
   * parent.
   * 
   * @param parent The parent of the list.
   */
  object_list() {}
  virtual ~object_list() {}

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // create and insert new item
      parent_ref pref(this->owner());
      item_type *it = new item_type(pref, x);
      item_ptr item = this->ostore()->insert(it);
      // mark list serializable as modified
//      this->mark_modified(this->parent());
      this->mark_modified(this->owner());
      // insert new item serializable
      return this->list().insert(pos, item);
    }
  }

  virtual iterator erase(iterator i)
  {
    if (!this->ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      item_ptr item = *i;
      this->mark_modified(this->owner());
//      this->mark_modified(this->parent());
      this->ostore()->remove(item);
      return this->list().erase(i);
    }
  }

protected:
///@cond OOS_DEV
  virtual object_base_producer* create_item_producer() const
  {
    return new object_producer<item_type>();
  }
///@endcond
};

#endif /* OOS_DOXYGEN_DOC */

}

#endif /* OBJECT_LIST_HPP */
