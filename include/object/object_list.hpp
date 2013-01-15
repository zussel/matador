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

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <list>
#include <algorithm>

namespace oos {

/**
 * @class object_list_base
 * @brief Base class for all object list classes.
 * @tparam S The type of the parent object.
 * @tparam T The value of the list.
 * @tparam CT The container item type.
 * 
 * The object_list class implements a list which
 * can hold any type of object from builtin types as
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
  typedef CT item_holder;                                    /**< Shortcut for the value holder type. */
  typedef typename CT::object_type item_type;                /**< Shortcut for the item type. */
  typedef std::list<item_holder> list_type;                  /**< Shortcut for the list class member. */
  typedef typename object_container::size_type size_type;    /**< Shortcut for size type. */
  typedef typename list_type::iterator iterator;             /**< Shortcut for the list iterator. */
  typedef typename list_type::const_iterator const_iterator; /**< Shortcut for the list const iterator. */

  /**
   * Create an empty object list
   * with the given parent object.
   *
   * @param parent The parent object of the list.
   */
  explicit object_list_base(S *parent)
    : parent_(parent)
  {}

  virtual ~object_list_base() {}
  
  /**
   * Return the class name of the item.
   * 
   * @return The class name of the item.
   */
  virtual const char* classname() const
  {
    return typeid(item_type).name();
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

  /**
   * Returns the parent object.
   * 
   * @return The parent object.
   */
  S* parent()
  {
    return parent_;
  }

  /**
   * Return the underlying list object.
   * 
   * @return The list object.
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

  S *parent_;
};

///@cond OOS_DEV

/*
 * helper class
 */
struct dummy { struct inner {}; typedef inner object_type; };

/*
 * Not implemented class
 */
template < class S, class T, void (oos::conditional<std::is_base_of<object_base_ptr, T>::value, T, dummy>::type::object_type::* ...SETFUNC)(const object_ref<S>&) >
struct object_list;

///@endcond

/**
 * @brief List class mapping items via relation table
 * 
 * @tparam S The parent class type.
 * @tparam T The item class type.
 * @tparam SETFUNC The parent setter function.
 *
 * This object list class uses no relation table. The items
 * hold the information of its parent by themself.
 */
template < class S, class T, void (T::object_type::*SETFUNC)(const object_ref<S>&)>
class object_list<S, T, SETFUNC> : public object_list_base<S, T>
{
public:
  typedef object_list_base<S, T> base_list;                  /**< Shortcut for the base list. */
  typedef object_ref<S> parent_ref;                          /**< Shortcut for the parent reference. */
  typedef typename T::object_type item_type;                 /**< Shortcut for the item type. */
  typedef typename base_list::value_holder value_holder;     /**< Shortcut for the value holder. */
  typedef typename base_list::item_holder item_holder;       /**< Shortcut for the item holder. */
  typedef typename base_list::size_type size_type;           /**< Shortcut for the size type. */
  typedef typename base_list::iterator iterator;             /**< Shortcut for the iterator. */
  typedef typename base_list::const_iterator const_iterator; /**< Shortcut for the const iterator. */

public:
  /**
   * Creates an empty list with the given
   * parent.
   * 
   * @param parent The parent of the list.
   */
  object_list(S *parent)
    : object_list_base<S, T>(parent)
    , setter_(SETFUNC)
  {}
  virtual ~object_list() {}

  /**
   * Return the class name of the item.
   * 
   * @return The class name of the item.
   */
  virtual const char* classname() const
  {
    return typeid(item_type).name();
  }

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // mark item object as modified
      this->mark_modified(x.get());
      // set back ref to parent
      setter_(*x.get(), parent_ref(this->parent()));
      // insert new item object
      return this->list().insert(pos, x);
    }
  };

  virtual iterator erase(iterator i)
  {
    if (!this->ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // mark item object as modified
      this->mark_modified((*i).get());
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
 * @brief List class mapping items via relation table
 * 
 * @tparam S The parent class type.
 * @tparam T The item class type.
 *
 * This object lisz class uses a relation table to
 * map the items to its parent.
 */
template < class S, class T>
class object_list<S, T> : public object_list_base<S, T, object_ptr<container_item<T, S> > >
{
public:
  typedef object_list_base<S, T, object_ptr<container_item<T, S> > > base_list; /**< Shortcut for the base list. */
  typedef T value_holder;                                                       /**< Shortcut for the value holder. */
  typedef object_ref<S> parent_ref;                                             /**< Shortcut for the parent reference. */
  typedef container_item<T, S> item_type;                                       /**< Shortcut for the item type. */
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
  object_list(S *parent)
    : object_list_base<S, T, object_ptr<container_item<T, S> > >(parent)
  {}
  virtual ~object_list() {}

  virtual iterator insert(iterator pos, const value_holder &x)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      // create and insert new item
      item_ptr item = this->ostore()->insert(new item_type(parent_ref(this->parent()), x));
      // mark list object as modified
      this->mark_modified(this->parent());
      // insert new item object
      return this->list().insert(pos, item);
    }
  }

  virtual iterator erase(iterator i)
  {
    if (!this->ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      item_ptr item = *i;
      this->mark_modified(this->parent());
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

}

#endif /* OBJECT_LIST_HPP */
