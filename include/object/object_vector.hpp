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
  /*
  virtual void read_from(oos::object_reader *reader)
  {
    base_item::read_from(reader);
    reader->read("item_index", index_);
  }
  virtual void write_to(oos::object_writer *writer) const
  {
    base_item::write_to(writer);
    writer->write("item_index", index_);
  }
  */
  template < class S >
  void deserialize(S &deserializer)
  {
    base_item::deserialize(deserializer);
    deserializer.read("item_index", index_);
  }
  template < class S >
  void serialize(S &serializer)
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
 * @class object_vector
 * @brief An object vector class.
 * @tparam S The type of the parent object.
 * @tparam T The value of the vector.
 * 
 * The object_vector class implements a vector which
 * can hold any type of object from builtin types as
 * int, float to object_ptr or object_ref elements.
 * The class provides STL like behaviour and the order of
 * the elements is reliable.
 */
template < typename S, class T >
class object_vector : public object_container
{
public:
  typedef T value_type;                                             /**< Shortcut for the value type. */
  typedef S container_type;                                         /**< Shortcut for the container type. */
  typedef object_vector_item<value_type, container_type> item_type; /**< Shortcut for the container item. */
  typedef object_ptr<item_type> item_ptr;                           /**< Shortcut for the container item pointer. */
  typedef std::vector<item_ptr> vector_type;                        /**< Shortcut for the vector class member. */
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
      iterator last = object_vector_.end();
      typename item_type::size_type index = object_vector_.size();
      if (index && pos != last) {
        index = (*pos)->index();
      }
      // create and insert new item
      item_ptr item = ostore()->insert(new item_type(object_ref<container_type>(parent_), index, x));
      // mark list object as modified
      mark_modified(parent_);
      // insert new item object
      pos = object_vector_.insert(pos, item);
      iterator first = pos;
      // adjust indices of successor items
      last = object_vector_.end();
      while (++first != last) {
        (*first)->index(++index);
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
    adjust_index(ret);

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
    iterator ret = object_vector_.erase(first, last);
    // adjust index
    adjust_index(ret);

    return ret;
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
    S *temp = dynamic_cast<S*>(p);
    if (!temp) {
      throw object_exception("couldn't cast object to concrete type");
    }
    parent_ = temp;
  }

private:
  virtual void append_proxy(object_proxy *proxy)
  {
    object_vector_.push_back(item_ptr(proxy));
  }

  void adjust_index(iterator i)
  {
    while (i != object_vector_.end()) {
      size_type index = 0;
      if (!(*i)->get("item_index", index)) {
        throw object_exception("couldn't get object vector index item");
//      } else if (!(*i)->set("item_index", --index)) {
//        throw object_exception("couldn't set object vector index item");
      } else {
        (*i)->index(--index);
        ++i;
      }
    }
  }

private:
  S *parent_;
  vector_type object_vector_;
};

}

#endif /* OBJECT_VECTOR_HPP */
