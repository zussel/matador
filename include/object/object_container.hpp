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

#ifndef OBJECT_CONTAINER_HPP
#define OBJECT_CONTAINER_HPP

#include "object/object.hpp"

namespace oos {

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
    , index_(0)
  {}
  container_item(const container_ref &c, const value_type &v)
    : container_(c)
    , index_(0)
    , value_(v)
  {}
  virtual ~container_item() {}

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

  size_type index() const
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

/**
 * @cond OOS_DEV
 * @class object_container
 * @brief Base class of all container types
 * 
 * This is the base class of all container types.
 */
class OOS_API object_container
{
public:
  typedef std::tr1::function<void (object *)> node_func; /**< Shortcut to the function type of the for_each method. */

public:
  /**
   * @brief Creates an empty object_container object.
   * 
   * The constructor creates an empty object_container.
   */
  object_container();

	virtual ~object_container();

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
  object_store* ostore() const;

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
   * Provides an interface which is called
   * when inserting the list into the object_store.
   */
  virtual void install(object_store *ostore);

  /**
   * Provides an interface which is called
   * when removing the list from the object_store.
   */
  virtual void uninstall();  
  
  virtual void parent(object *p) = 0;

private:
  virtual void reset() {}

private:
  object_store *ostore_;
};
/// @endcond

}

#endif /* OBJECT_CONTAINER_HPP */
