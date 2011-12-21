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

#include <vector>

namespace oos {

/**
 * @class object_vector_base
 * @brief Base class of all list types
 * 
 * This is the base class of all list types.
 */
class OOS_API object_vector_base
{
public:
  typedef std::tr1::function<void (object *)> node_func; /**< Shortcut to the function type of the for_each method. */

public:
  /**
   * @brief Creates an empty object_vector_base object.
   * 
   * The constructor creates an empty object_vector_base
   * object which is part of the given parent object. This
   * means that the parent object contains this vector and
   * each node object must have a reference field/parameter
   * for the parent object set automatically on insert. Also
   * the index of the element is stored into a parameter field
   * with the given name.
   * 
   * @param parent The parent object of this vector.
   * @param vector_name The name of the vector reference parameter.
   * @param index_name The name of the index parameter.
   */
  object_vector_base(object *parent, const std::string &vector_name, const std::string &index_name);

	virtual ~object_vector_base();

  /**
   * Tell wether the vector is empty or not.
   * 
   * @return Returns true if the vector is empty.
   */
  virtual bool empty() const = 0;

  /**
   * @brief Clears the vector.
   * 
   * Removes every element from the vector and
   * subsequently from the object_store.
   */
  virtual void clear() = 0;
  
  /**
   * Returns the size of the vector.
   * 
   * @return The size of the vector.
   */
  virtual size_t size() const = 0;

  /**
   * @brief Returns the containing object_store.
   * 
   * Returns the containing object_store. If the vector
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
   * @brief Push front a object via its object_proxy.
   *
   * Push front a object via its object_proxy to the vector.
   *
   * @param op The object_proxy containing the list element object.
   */
  virtual void push_front_proxy(object_proxy *op) = 0;

  /**
   * @brief Push back a object via its object_proxy.
   *
   * Push back a object via its object_proxy to the vector.
   *
   * @param op The object_proxy containing the list element object.
   */
  virtual void push_back_proxy(object_proxy *op) = 0;

  /**
   * Mark the vector containing object as modified
   * in the object_store.
   *
   * @param o The object containig vector
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
   * Returns the vector containing object.
   *
   * @return The vector containing object.
   */
  object* parent_object() const { return parent_; }

  /**
   * Sets the vector containing object.
   *
   * @param parent The vector containing object.
   */
  void parent_object(object *parent) { parent_ = parent; }

  /**
   * Provides an interface which is called
   * when inserting the vector into the object_store.
   */
  virtual void install(object_store *ostore);

  /**
   * Provides an interface which is called
   * when removing the vector from the object_store.
   */
  virtual void uninstall();  

  /**
   * @brief Sets the vector reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @return True if the value could be set.
   */
  bool set_reference(object *elem);

  /**
   * Return the name of the vector parameter.
   *
   * @return The name of the vector parameter.
   */
  std::string list_name() const
  {
    return vector_name_;
  }

  /**
   * Return the name of the index parameter.
   *
   * @return The name of the index parameter.
   */
  std::string index_name() const
  {
    return index_name_;
  }
  
private:
  virtual void reset() {}

private:
  object_store *ostore_;
  object *parent_;
  std::string vector_name_;
  std::string index_name_;
};

/**
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
template < typename T, class W >
class object_vector : public object_vector_base
{
public:
  typedef object_vector_base base_vector;                      /**< Shortcut for the object_base_vector class. */
  typedef T value_type;                                        /**< Shortcut for the value type. */
  typedef W value_type_wrapper;                                /**< Shortcut for the wrapper class around the value type. */
  typedef std::vector<value_type_wrapper> vector_type;         /**< Shortcut for the vector class member. */
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
   * @param parent The containing vector object.
   * @param vector_name The name of the parent in the value type object.
   * @param index_name The name of the index parameter.
   */
  object_vector(object *parent, const std::string &vector_name, const std::string &index_name)
    : object_vector_base(parent, vector_name, index_name)
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
    base_vector::clear();
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
  iterator insert(iterator pos, const value_type_wrapper &x)
  {
    if (!base_vector::ostore()) {
      //throw object_exception();
      return pos;
    } else {
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
        return pos;
      } else {
        // mark list object as modified
        base_vector::mark_modified(parent_object());
        // insert new item object
        return object_vector_.insert(pos, x);
      }
    }
  };

  /**
   * Adds an element to the beginning of the list.
   *
   * @param x The element to be pushed front.
   */
  void push_front(const value_type_wrapper &x)
  {
    if (!base_vector::ostore()) {
      //throw object_exception();
    } else {
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
      } else {
        // mark list object as modified
        base_vector::mark_modified(parent_object());
        // insert new item object
        object_vector_.push_front(x);
      }
    }
  }

  /**
   * Adds an element to the end of the list.
   *
   * @param x The element to be pushed back.
   */
  void push_back(const value_type_wrapper &x)
  {
    if (!base_vector::ostore()) {
      //throw object_exception();
    } else {
      // set reference
      if (!set_reference(x.get())) {
        // throw object_exception();
      } else {
        // mark list object as modified
        base_vector::mark_modified(parent_object());
        // insert new item object
        object_vector_.push_back(x);
      }
    }
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  value_type_wrapper operator[](size_type pos)
  {
    return object_vector_[pos];
  }

  /**
   * Access the element at given position.
   *
   * @param pos The index of the requested element.
   * @return The requested element.
   */
  const value_type_wrapper operator[](size_type pos) const
  {
    return object_vector_[pos];
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
    return object_vector_.erase(i);
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

private:
  vector_type object_vector_;
};

}

#endif /* OBJECT_VECTOR_HPP */