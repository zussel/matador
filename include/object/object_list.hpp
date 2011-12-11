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
#include "object/object_linker.hpp"
#include "object/object_atomizer.hpp"

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
   */
  object_list_base(object *parent);

	virtual ~object_list_base();

  /**
   * Reads the data in from the given object_atomizer.
   * 
   * @param ao The object_atomizer to read from.
   */
	//virtual void read_from(object_atomizer *ao) = 0;

  /**
   * Writes the data to the given object_atomizer.
   * 
   * @param ao The object_atomizer to write to.
   */
	//virtual void write_to(object_atomizer *ao) const = 0;

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
   * @param 
   */
  virtual void push_front_proxy(object_proxy *) = 0;
  virtual void push_back_proxy(object_proxy *) = 0;

  // mark modified object containig the list
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

private:
  virtual void reset() {}

private:
  object_store *ostore_;
  object *parent_;
};

/**
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
template < typename T, class W >
class object_list : public object_list_base
{
public:
  typedef object_list_base base_list;                         /**< Shortcut for the object_base_list class. */
  typedef T value_type;                                       /**< Shortcut for the value type. */
  typedef W value_type_wrapper;                               /**< Shortcut for the wrapper class around the value type. */
  typedef std::list<value_type_wrapper> list_type;            /**< Shortcut for the list class member. */
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
    : object_list_base(parent)
    , list_name_(list_ref_name)
  {}

  virtual ~object_list() {}
  
  /*
	virtual void read_from(object_atomizer *) {}
	virtual void write_to(object_atomizer *) const {}
  */

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
  virtual size_t size() const
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
  iterator insert(iterator pos, const value_type_wrapper &x)
  {
    return object_list_.insert(pos, x);
  };

  /**
   * @brief Interface to erase an element
   *
   * This is the interface for derived object_list
   * classes to implement thier erase element method.
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
   * @brief Sets the list reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @param o The parent list object to be set in the child element.
   * @return True if the value could be set.
   */
  bool set_reference(value_type *elem, const object_base_ptr &o)
  {
    object_linker ol(elem, o, list_name_);
    elem->read_from(&ol);
    return ol.success();
  }

  /**
   * Return the name of the list parameter.
   *
   * @return The name of the list parameter.
   */
  std::string list_name() const { return list_name_; }

  /**
   * Adds an element to the beginning of the list.
   *
   * @param x The element to be pushed front.
   */
  void push_front(const value_type_wrapper &x)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // mark list object as modified
      base_list::mark_modified(o.ptr());
      // set reference
      if (!set_reference(elem)) {
        // throw object_exception();
      } else {
        // insert new item object
        value_type_ptr optr = base_list::ostore()->insert(elem);
        // and call base list class push back
        base_list::push_front(optr);
      }
    }
    object_list_.push_front(x);
  }

  /**
   * Adds an element to the end of the list.
   *
   * @param x The element to be pushed back.
   */
  void push_back(const value_type_wrapper &x)
  {
    object_list_.push_back(x);
  }

  /**
   * Erases a single element from the list.
   *
   * @param i The element to remove.
   * @return Returns the element at the following position.
   */
  iterator erase_i(iterator i) { return object_list_.erase(i); }
  
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

  virtual void push_front_proxy(object_proxy *proxy)
  {
    object_list_.push_front(value_type_wrapper(proxy));
  }

  virtual void push_back_proxy(object_proxy *proxy)
  {
    object_list_.push_back(value_type_wrapper(proxy));
  }

private:
  list_type object_list_;
  std::string list_name_;
};

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
class object_ptr_list : public object_list<T, object_ptr<T> >
{
public:
  typedef object_list<T, object_ptr<T> > base_list;              /**< Shortcut for the object_list class. */
  typedef typename base_list::value_type value_type;             /**< Shortcut for the value type. */
  typedef typename base_list::value_type_wrapper value_type_ptr; /**< Shortcut for the wrapper class around the value type. */
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

  /**
   * @brief Push a new object to the front of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed front to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object is done automatilcally.
   *
   * @param elem The element to be pushed front
   * @param o The list containing object (parent)
   */
  virtual void push_front(T *elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // mark list object as modified
      base_list::mark_modified(o.ptr());
      // set reference
      if (!set_reference(elem, o)) {
        // throw object_exception();
      } else {
        // insert new item object
        value_type_ptr optr = base_list::ostore()->insert(elem);
        // and call base list class push back
        base_list::push_front(optr);
      }
    }
  }

  /**
   * @brief Push a new object to the end of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed back to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object is done automatilcally.
   *
   * @param elem The element to be pushed back
   * @param o The list containing object (parent)
   */
  virtual void push_back(T* elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // mark list object as modified
      base_list::mark_modified(o.ptr());
      // set reference
      if (!set_reference(elem, o)) {
        // throw object_exception();
      } else {
        // insert new item object
        value_type_ptr optr = base_list::ostore()->insert(elem);
        // and call base list class push back
        base_list::push_back(optr);
      }
    }
  }

  /**
   * @brief Erase the object at iterators position.
   * 
   * The object inside the iterator will first be
   * removed from the object_store and second be erased
   * from the list.
   * The next iterator position is returned.
   *
   * @param i The object to be erased containing iterator.
   * @return The next iterator position
   */
  virtual iterator erase(iterator i)
  {
    if (!base_list::ostore()) {
      // if list is not in ostore
      // throw exception
      //throw object_exception();
      return i;
    }
    // update predeccessor and successor
    value_type_ptr optr = *i;
    if (!base_list::ostore()->remove(optr)) {
      // throw exception
      return i;
    } else {
      // object was successfully deleted
      return base_list::erase_i(i);
    }
  }
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
class object_ref_list : public object_list<T, object_ref<T> >
{
public:
  typedef object_list<T, object_ref<T> > base_list;              /**< Shortcut for the object_list class. */
  typedef typename base_list::value_type value_type;             /**< Shortcut for the value type. */
  typedef typename base_list::value_type_wrapper value_type_ref; /**< Shortcut for the wrapper class around the value type. */
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

  /**
   * @brief Push an existing object reference to the front of the list.
   * 
   * An object reference is pushed front to the list.
   * Furthermore the reference link to the list object
   * is done automatilcally.
   *
   * @param elem The element to be pushed front
   * @param o The list containing object (parent)
   */
  virtual void push_front(const value_type_ref &elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // set link to list object
      if (!set_reference(elem.get(), o)) {
        //throw object_exception();
      } else {
        base_list::push_front(elem);
      }
    }
  }

  /**
   * @brief Push an existing object reference to the end of the list.
   * 
   * An object reference is pushed back to the list.
   * Furthermore the reference link to the list object
   * is done automatilcally.
   *
   * @param elem The element to be pushed back
   * @param o The list containing object (parent)
   */
  virtual void push_back(const value_type_ref &elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // set link to list object
      if (!set_reference(elem.get(), o)) {
        //throw object_exception();
      } else {
        base_list::push_back(elem);
      }
    }
  }

  /**
   * @brief Erase the object at iterators position.
   * 
   * The object inside the iterator will first be erased
   * from the list.
   * The next iterator position is returned.
   *
   * @param i The object to be erased containing iterator.
   * @return The next iterator position
   */
  iterator erase(iterator i)
  {
    /***************
     * 
     * object references can't be
     * deleted from object store
     * only delete them from lists
     * internal object refenence list
     * 
     ***************/
    return base_list::erase_i(i);
  }
};

}

#endif /* OBJECT_LIST_HPP */
