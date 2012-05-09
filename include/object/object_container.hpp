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

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

namespace oos {

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
  typedef long unsigned int size_type;                             /**< Shortcut for size type. */

public:
  /**
   * @brief Creates an empty object_container object.
   * 
   * The constructor creates an empty object_container.
   */
  object_container()
    : ostore_(0)
  {}

  virtual ~object_container() {}

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
  virtual size_type size() const = 0;

  /**
   * @brief Returns the containing object_store.
   * 
   * Returns the containing object_store. If the list
   * isn't inserted into an object_store NULL is
   * returned.
   * 
   * @return The containing object_store.
   */
  object_store* ostore() const
  {
    return ostore_;
  }

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
  void mark_modified(object *o)
  {
    o->mark_modified();
  }

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
  virtual void install(object_store *ostore)
  {
    ostore_ = ostore;
  }

  /**
   * Provides an interface which is called
   * when removing the list from the object_store.
   */
  virtual void uninstall()
  {
    ostore_ = 0;
  }
  
  /**
   * Sets the parent for the
   * concrete container
   *
   * @param p The parent object of the container.
   */
  virtual void parent(object *p) = 0;

private:
  virtual void reset() {}

private:
  object_store *ostore_;
};
/// @endcond

}

#endif /* OBJECT_CONTAINER_HPP */
