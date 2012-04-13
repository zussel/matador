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

/**
 * @cond OOS_DEV
 * @class object_list_base
 * @brief Base class of all list types
 * 
 * This is the base class of all list types.
 */
class OOS_API object_list_base
{
public:
  typedef std::tr1::function<void (object *)> node_func; /**< Shortcut to the function type of the for_each method. */

public:
  /**
   * @brief Creates an empty object_list_base object.
   * 
   * The constructor creates an empty object_list_base
   * object which is part of the given parent object. This
   * means that the parent object contains this list and
   * each node object must have a reference field/parameter
   * for the parent object set automatically on insert.
   * 
   * @param parent The parent object of this list.
   * @param list_ref_name The name of the list reference parameter.
   */
  object_list_base(object *parent, const std::string &list_ref_name);

	virtual ~object_list_base();

  /**
   * Tell wether the list is empty or not.
   * 
   * @return Returns true if the list is empty.
   */
  virtual bool empty() const = 0;

  /**
   * @brief Clears the list.
   * 
   * Removes every element from the list and
   * subsequently from the object_store.
   */
  virtual void clear() = 0;
  
  /**
   * Returns the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_t size() const = 0;

  /**
   * @brief Returns the containing object_store.
   * 
   * Returns the containing object_store. If the list
   * isn't inserted into an object_store NULL is
   * returned.
   * 
   * @return The containing object_store.
   */
  object_store* ostore() const { return ostore_; }

protected:
  friend class object_store;
  friend class object_creator;
  friend class object_deleter;
  friend class object_serializer;

  /**
   * @brief Append a object via its object_proxy.
   *
   * Append a object via its object_proxy to the list.
   *
   * @param op The object_proxy containing the list element object.
   */
  virtual void append_proxy(object_proxy *op) = 0;

  /**
   * Mark the list containing object as modified
   * in the object_store.
   *
   * @param o The object containig list
   */
  void mark_modified(object *o);

  /**
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const = 0;

  /**
   * Returns the list containing object.
   *
   * @return The list containing object.
   */
  object* parent_object() const { return parent_; }

  /**
   * Sets the list containing object.
   *
   * @param parent The list containing object.
   */
  void parent_object(object *parent) { parent_ = parent; }

  /**
   * Provides an interface which is called
   * when inserting the list into the object_store.
   */
  virtual void install(object_store *ostore);

  /**
   * Provides an interface which is called
   * when removing the list from the object_store.
   */
  virtual void uninstall();  

  /**
   * @brief Sets the list reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @return True if the value could be set.
   */
  bool set_reference(object *elem);

  /**
   * Return the name of the list parameter.
   *
   * @return The name of the list parameter.
   */
  std::string list_name() const
  {
    return list_name_;
  }
  
private:
  virtual void reset() {}

private:
  object_store *ostore_;
  object *parent_;
  std::string list_name_;
};
/// @endcond

/**
 * @cond OOS_DEV
 * @class object_list
 * @brief An object list class.
 * @tparam T The concrete object type.
 * @tparam W The object pointer or reference class.
 * 
 * The object_list class is the base class for
 * The object_ptr_list class and the object_ref_list
 * class.
 * It stores a wrapper (object_ptr or object_ref) object
 * around the concrete object type inside a list.
 * The class provides STL like behaviour and the order of
 * the elements is not reliable.
 */
template < typename T >
class object_list : public object_list_base
{
public:
  typedef object_list_base base_list;                         /**< Shortcut for the object_base_list class. */
  typedef T value_type;                                       /**< Shortcut for the wrapper class around the value type. */
  typedef std::list<value_type> list_type;                    /**< Shortcut for the list class member. */
  typedef typename list_type::size_type size_type;            /**< Shortcut for size type. */
  typedef typename list_type::iterator iterator;              /**< Shortcut for the list iterator. */
  typedef typename list_type::const_iterator const_iterator;  /**< Shortcut for the list const iterator. */

  /**
   * @brief Creates a new object_list.
   * 
   * A new object_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  object_list(object *parent, const std::string &list_ref_name)
    : object_list_base(parent, list_ref_name)
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
    base_list::clear();
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
    if (!base_list::ostore()) {
      //throw object_exception();
      return pos;
    } else {
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
        return pos;
      } else {
        // mark list object as modified
        base_list::mark_modified(parent_object());
        // insert new item object
        return object_list_.insert(pos, x);
      }
    }
  };

  /**
   * Adds an element to the beginning of the list.
   *
   * @param x The element to be pushed front.
   */
  void push_front(const value_type &x)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
      } else {
        // mark list object as modified
        base_list::mark_modified(parent_object());
        // insert new item object
        object_list_.push_front(x);
      }
    }
  }

  /**
   * Adds an element to the end of the list.
   *
   * @param x The element to be pushed back.
   */
  void push_back(const value_type &x)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
      } else {
        // mark list object as modified
        base_list::mark_modified(parent_object());
        // insert new item object
        object_list_.push_back(x);
      }
    }
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
    return object_list_.erase(i);
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

  virtual void uninstall()
  {
    base_list::uninstall();
    object_list_.clear();
  }

private:
  virtual void reset()
  {
    object_list_.clear();
  }

  virtual void append_proxy(object_proxy *proxy)
  {
    object_list_.push_back(value_type(proxy));
  }

private:
  list_type object_list_;
};
/// @endcond

/**
 * @class object_ptr_list
 * @brief An object list class for object_ptr
 * @tparam T The concrete object type.
 * 
 * This is a specialisation of the object_list class.
 * It stores object_ptr in a list. The raw objects
 * are inserted into the list and than subsequently
 * inserted into the object_store. The reference link
 * to the parent class is done automatically.
 */
template < typename T >
class object_ptr_list : public object_list<object_ptr<T> >
{
public:
  typedef object_list<object_ptr<T> > base_list;              /**< Shortcut for the object_list class. */
  typedef typename base_list::size_type size_type;               /**< Shortcut for size type- */
  typedef typename base_list::value_type value_type;             /**< Shortcut for the value type. */
  typedef typename base_list::iterator iterator;                 /**< Shortcut for the list iterator. */
  typedef typename base_list::const_iterator const_iterator;     /**< Shortcut for the list const iterator. */

  /**
   * @brief Creates an empty list.
   * 
   * A new object_ptr_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  object_ptr_list(object *parent, const std::string &list_ref_name)
    : base_list(parent, list_ref_name)
  {}
  
  virtual ~object_ptr_list() {}
};

/**
 * @class object_ref_list
 * @brief An object list class for object_ref
 * @tparam T The concrete object type.
 * 
 * This is a specialisation of the object_list class.
 * It stores object_ref in a list. The raw objects
 * are inserted into the list and than subsequently
 * inserted into the object_store. The reference link
 * to the parent class is done automatically.
 */
template < typename T >
class object_ref_list : public object_list<object_ref<T> >
{
public:
  typedef object_list<object_ref<T> > base_list;              /**< Shortcut for the object_list class. */
  typedef typename base_list::size_type size_type;               /**< Shortcut for size type- */
  typedef typename base_list::value_type value_type;             /**< Shortcut for the value type. */
  typedef typename base_list::iterator iterator;                 /**< Shortcut for the list iterator. */
  typedef typename base_list::const_iterator const_iterator;     /**< Shortcut for the list const iterator. */

  /**
   * @brief Creates an empty list.
   * 
   * A new object_ref_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  object_ref_list(object *parent, const std::string &list_ref_name)
    : base_list(parent, list_ref_name)
  {}

  virtual ~object_ref_list() {}
};

template < class S, class T >
class object_list_2 : public object_container
{
  typedef object_list_2<S, T> self;
  typedef T value_type;                                       /**< Shortcut for the wrapper class around the value type. */
  typedef container_item<value_type, self> item_type;
  typedef object_ptr<item_type> item_ptr;
  typedef std::list<item_ptr> list_type;                      /**< Shortcut for the list class member. */
  typedef typename list_type::size_type size_type;            /**< Shortcut for size type. */
  typedef typename list_type::iterator iterator;              /**< Shortcut for the list iterator. */
  typedef typename list_type::const_iterator const_iterator;  /**< Shortcut for the list const iterator. */


  explicit object_list_2(S *parent)
    : parent_(parent)
  {}

  virtual ~object_list_2() {}
  
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
      item_ptr item = ostore()->insert(new item_type(object_ref<self>(this), x));
      /*
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
        return pos;
      } else {
      */
        // mark list object as modified
      base_list::mark_modified(parent_object());
//        item_type *item = new 
        // insert new item object
      return object_list_.insert(pos, item);
//      }
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
    return object_list_.erase(i);
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

  virtual void uninstall()
  {
    base_list::uninstall();
    object_list_.clear();
  }

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
//    object_list_.push_back(value_type(proxy));
  }

private:
  list_type object_list_;

  S *parent_;
};

}

#endif /* OBJECT_LIST_HPP */
