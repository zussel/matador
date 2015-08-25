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

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "object/primary_key_serializer.hpp"
#include "prototype_node.hpp"

#include <ostream>
#include <set>
#include <list>
#include <map>

#include <memory>

namespace oos {

class serializable;
class object_store;
class object_base_ptr;
class prototype_node;
class primary_key_base;

/**
 * @cond OOS_DEV
 * @class object_proxy
 * @brief A proxy between the serializable and the object_store
 *
 * Is a proxy between the serializable and the serializable store. It stores
 * the pointer to the serializable and the id. Once a new serializable
 * is inserted into the 
 */
class object_proxy {
public:
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
   * Create a new object proxy with primary key and node
   *
   * @param pk primary key of object
   * @param node The prototype node
   */
  object_proxy(const std::shared_ptr<primary_key_base> &pk, prototype_node *node);

  /**
   * @brief Create an object_proxy for unknown
   *        serializable with given id.
   *
   * Create an object_proxy for unknown serializable
   * with given id.
   *
   * @param i The id of the expected serializable.
   * @param os The object_store.
   */
  object_proxy(unsigned long i, object_store *os);

  /**
   * @brief Create an object_proxy for a given serializable.
   *
   * Create an object_proxy for unknown serializable
   * with given id.
   *
   * @param o The valid serializable.
   * @param os The object_store.
   */
  object_proxy(serializable *o, object_store *os);

  /**
   * @brief Create an object_proxy for a given serializable.
   *
   * Create an object_proxy for unknown serializable
   * with given id.
   *
   * @param o The valid serializable.
   * @param id The object store id for the given object
   * @param os The object_store.
   */
  object_proxy(serializable *o, unsigned long id, object_store *os);

  ~object_proxy();

  /**
   * Return the underlaying serializable object
   *
   * @return The underlaying serializable
   */
  serializable* obj();

  /**
   * Return the underlaying serializable object
   *
   * @return The underlaying serializable
   */
  const serializable* obj() const;

  /**
   * Return the underlaying object store
   *
   * @return The object store
   */
  object_store* ostore() const;

  /**
   * Return the corresponding prototype
   * node.
   *
   * @return The corresponding prototype node
   */
  prototype_node* node() const;

  /**
   * Print the object_proxy to a stream
   *
   * @param os The stream where the object_proxy is printed to
   * @param op The object_proxy to print
   * @return The modified stream.
   */
  friend OOS_API std::ostream& operator <<(std::ostream &os, const object_proxy &op);

  /**
   * Link this serializable proxy before given
   * serializable proxy next.
   * 
   * @param successor New next serializable proxy of this
   */
  void link(object_proxy *successor);

  /**
   * @brief Unlink serializable proxy from list.
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
   * Return the next object proxy
   *
   * @return The next object proxy
   */
  object_proxy* next() const;

  /**
   * Return the previous object proxy
   *
   * @return The previous object proxy
   */
  object_proxy* prev() const;

  /**
   * Return the current reference count
   * for underlaying serializable
   *
   * @return Current reference count
   */
  unsigned long ref_count() const;

  /**
   * Return the current pointer count
   * for underlaying serializable
   *
   * @return Current pointer count
   */
  unsigned long ptr_count() const;

  /**
   * Resets the serializable of the object_proxy
   * with the given serializable.
   *
   * @param o The new serializable for the object_proxy
   */
  void reset(serializable *o);

  /**
   * @brief Add an object_base_ptr to the linked list.
   *
   * Each object_base_ptr containg this object_proxy
   * calls this method. So object_proxy knows how many
   * object_base_ptr are dealing with this serializable.
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
   * are dealing with this serializable.
   *
   * @param ptr The object_base_ptr containing this object_proxy.
   */
  bool remove(object_base_ptr *ptr);

  /**
   * @brief True if proxy is valid
   * 
   * Returns true if the proxy is part of
   * an object_store. Therefor ostore, node,
   * prev and next must be set.
   * 
   * @return True if part of an object_store.
   */
  bool valid() const;

  /**
   * Return the id of the serializable. If no serializable is
   * set 0 (null) is returned
   *
   * @return 0 (null) or the id of the serializable.
   */
  unsigned long id() const;
  void id(unsigned long i);

  /**
   * Returns true if the underlying
   * serializable has a primary key
   *
   * @return true If the serializable has a primary key
   */
  bool has_primary_key() const;

  /**
   * Return the primary key. If underlaying serializable
   * doesn't have a primary key, an uninitialized
   * primary key shared ptr is returned
   *
   * @return The primary key of the underlaying serializable
   */
  std::shared_ptr<primary_key_base> pk() const;

private:
  friend class object_store;
  friend class object_preparator;
  friend class prototype_node;
  friend class prototype_tree;
  friend class result;
  friend class table_reader;
  friend class restore_visitor;

  object_proxy *prev_;      /**< The previous object_proxy in the list. */
  object_proxy *next_;      /**< The next object_proxy in the list. */

  serializable *obj_;        /**< The concrete serializable. */
  unsigned long oid;        /**< The id of the concrete or expected serializable. */

  unsigned long ref_count_; /**< The reference counter */
  unsigned long ptr_count_; /**< The pointer counter */

  object_store *ostore_;    /**< The object_store to which the object_proxy belongs. */
  prototype_node *node_;    /**< The prototype_node containing the type of the serializable. */

  typedef std::set<object_base_ptr*> ptr_set_t; /**< Shortcut to the object_base_ptr_set. */
  ptr_set_t ptr_set_;      /**< This set contains every object_base_ptr pointing to this object_proxy. */
  
  typedef std::list<serializable*> object_list_t;
  typedef std::map<std::string, object_list_t> string_object_list_map_t;
  
  string_object_list_map_t relations;

  std::shared_ptr<primary_key_base> primary_key_ = nullptr;

  static primary_key_serializer pk_serializer;
};
/// @endcond
}

#endif /* OBJECT_PROXY_HPP */
