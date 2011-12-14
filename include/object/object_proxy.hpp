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

#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

#include <ostream>
#include <set>

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace oos {

class object;
class object_store;
class object_base_ptr;
struct prototype_node;

/**
 * @class object_proxy
 * @brief A proxy between the object and the object_store
 *
 * Is a proxy between the object and the object store. It stores
 * the pointer to the object and the id. Once a new object
 * is inserted into the 
 */
struct object_proxy {
  /**
   * @brief Create an empty object_proxy
   *
   * Create an empty object_proxy for a
   * given object_store
   *
   * @param os The object_store.
   */
  explicit object_proxy(object_store *os);

  /**
   * @brief Create an object_proxy for unknown
   *        object with given id.
   *
   * Create an object_proxy for unknown object
   * with given id.
   *
   * @param i The id of the expected object.
   * @param os The object_store.
   */
  object_proxy(long i, object_store *os);

  /**
   * @brief Create an object_proxy for a given object.
   *
   * Create an object_proxy for unknown object
   * with given id.
   *
   * @param o The valid object.
   * @param os The object_store.
   */
  object_proxy(object *o, object_store *os);

  ~object_proxy();

  /**
   * Print the object_proxy to a stream
   *
   * @param os The stream where the object_proxy is printed to
   * @param op The object_proxy to print
   * @return The modified stream.
   */
  friend std::ostream& operator <<(std::ostream &os, const object_proxy &op);

  /**
   * Link this object proxy before given
   * object proxy next.
   * 
   * @param successor New next object proxy of this
   */
  void link(object_proxy *successor);

  /**
   * @brief Unlink object proxy from list.
   *
   * When an object_proxy is unlinked it
   * is removed from the object_proxy list
   * contained by the object_store.
   *
   * Maybe the markers are adjusted.
   */
  void unlink();

  /**
   * @brief Link as referenece
   *
   * Link as referenece incremenets
   * the reference counter.
   */
  void link_ref();

  /**
   * @brief Unlink as referenece
   *
   * Unlink as referenece decremenets
   * the reference counter.
   */
  void unlink_ref();

  /**
   * @brief Link as pointer
   *
   * Link as pointer incremenets
   * the pointer counter.
   */
  void link_ptr();

  /**
   * @brief Unlink as pointer
   *
   * Unlink as pointer decremenets
   * the pointer counter.
   */
  void unlink_ptr();

  /**
   * Return true if the object_proxy is linked.
   *
   * @return True if the object_proxy is linked.
   */
  bool linked() const;

  /**
   * Resets the object of the object_proxy
   * with the given object.
   *
   * @param o The new object for the object_proxy
   */
  void reset(object *o);

  /**
   * @brief Add an object_base_ptr to the linked list.
   *
   * Each object_base_ptr containg this object_proxy
   * calls this method. So object_proxy knows how many
   * object_base_ptr are dealing with this object.
   *
   * @param ptr The object_base_ptr containing this object_proxy.
   */
  void add(object_base_ptr *ptr);

  /**
   * @brief Remove an object_base_ptr from the linked list.
   *
   * Each destroying ore reseting object_base_ptr
   * containg this object_proxy calls this method.
   * So object_proxy knows how many object_base_ptr
   * are dealing with this object.
   *
   * @param ptr The object_base_ptr containing this object_proxy.
   */
  bool remove(object_base_ptr *ptr);

  object_proxy *prev;      /**< The previous object_proxy in the list. */
  object_proxy *next;      /**< The next object_proxy in the list. */

  object *obj;             /**< The concrete object. */
  unsigned long id;        /**< The id of the concrete or expected object. */
  
  unsigned long ref_count; /**< The reference counter */
  unsigned long ptr_count; /**< The pointer counter */

  object_store *ostore;    /**< The object_store to which the object_proxy belongs. */
  prototype_node *node;    /**< The prototype_node containing the type of the object. */

  typedef std::set<object_base_ptr*> ptr_set_t; /**< Shortcut to the object_base_ptr_set. */
  ptr_set_t ptr_set_;      /**< This set contains every object_base_ptr pointing to this object_proxy. */
};

}

#endif /* OBJECT_PROXY_HPP */
