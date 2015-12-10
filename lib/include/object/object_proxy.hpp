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

#include "object/identifier_resolver.hpp"

#include <ostream>
#include <set>
#include <list>

#include <map>
#include <memory>

namespace oos {

class serializable;
class object_store;
class basic_object_holder;
class prototype_node;
class basic_identifier;

/**
 * @cond OOS_DEV
 * @class object_proxy
 * @brief A proxy between the serializable and the object_store
 *
 * Is a proxy between the serializable and the serializable store. It stores
 * the pointer to the serializable and the id. Once a new serializable
 * is inserted into the 
 */
class OOS_API object_proxy {
public:

  /**
   * @brief Create an empty object_proxy
   *
   * Create an empty object_proxy
   */
  object_proxy();

  /**
   * @brief Create an object_proxy for unknown
   *        serializable with given id.
   *
   * Create an object_proxy for unknown serializable
   * with given id.
   *
   * @param i The id of the unknown serializable.
   */
//  explicit object_proxy(unsigned long id);

  /**
   * Create a new object proxy with primary key
   *
   * @param pk primary key of object
   */
  explicit object_proxy(const std::shared_ptr<basic_identifier> &pk);

  /**
   * @brief Create an object_proxy for a given serializable.
   *
   * Create an object_proxy for unknown serializable
   * with given id.
   *
   * @param o The valid serializable.
   */
  template < typename T >
  explicit object_proxy(T *o)
    : obj_(o)
    , deleter_(&destroy<T>)
    , namer_(&type_id<T>)
  {
    primary_key_.reset(identifier_resolver<T>::resolve());
  }

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
  template < typename T >
  object_proxy(T *o, unsigned long id, object_store *os)
    : obj_(o)
    , deleter_(&destroy<T>)
    , namer_(&type_id<T>)
    , oid(id)
    , ostore_(os)
  {
    if (obj_ != nullptr) {
      primary_key_.reset(identifier_resolver<T>::resolve());
    }
  }

  ~object_proxy();

  /**
   * Return the classname/typeid of the object
   *
   * @return The classname of the object
   */
  const char* classname() const;

  /**
   * Return the underlaying object
   *
   * @tparam The type of the object
   * @return The underlaying object
   */
  template < typename T = void >
  T* obj()
  {
    return static_cast<T*>(obj_);
  }

  /**
   * Return the underlaying object
   *
   * @tparam The type of the object
   * @return The underlaying object
   */
  template < typename T = void >
  const T* obj() const
  {
    return static_cast<const T*>(obj_);
  }

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
  template < typename T >
  void reset(T *o)
  {
    ref_count_ = 0;
    ptr_count_ = 0;
    deleter_ = destroy;
    namer_ = type_id;
    obj_ = o;
    oid = 0;
    node_ = 0;
    if (obj_ != nullptr) {
      primary_key_.reset(identifier_resolver<T>::resolve());
    }
  }


  /**
   * @brief Add an basic_object_holder to the linked list.
   *
   * Each basic_object_holder containing this object_proxy
   * calls this method. So object_proxy knows how many
   * basic_object_holder are dealing with this serializable.
   *
   * @param ptr The basic_object_holder containing this object_proxy.
   */
  void add(basic_object_holder *ptr);

  /**
   * @brief Remove an basic_object_holder from the linked list.
   *
   * Each destroying ore reseting basic_object_holder
   * containg this object_proxy calls this method.
   * So object_proxy knows how many basic_object_holder
   * are dealing with this serializable.
   *
   * @param ptr The basic_object_holder containing this object_proxy.
   */
  bool remove(basic_object_holder *ptr);

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

  /**
   * Sets the id of the object_proxy.
   *
   * @param i New id of the proxy.
   */
  void id(unsigned long i);

  /**
   * Returns true if the underlying
   * serializable has a primary key
   *
   * @return true If the serializable has a primary key
   */
  bool has_identifier() const;

  /**
   * Return the primary key. If underlaying serializable
   * doesn't have a primary key, an uninitialized
   * primary key shared ptr is returned
   *
   * @return The primary key of the underlaying serializable
   */
  std::shared_ptr<basic_identifier> pk() const;

private:
  friend class object_store;
  friend class prototype_node;
  friend class prototype_tree;
  template < class T > friend class result;
  friend class table_reader;
  friend class restore_visitor;
  friend class basic_object_holder;
  template < class T > friend class has_one;

  typedef void (*deleter)(void*);
  typedef const char* (*namer)();

  template <typename T>
  static void destroy(void* p)
  {
    delete static_cast<T*>(p);
  }

  template < class T >
  static const char* type_id()
  {
    return typeid(T).name();
//    return typeid(static_cast<T*>(p)).name();
  }

  object_proxy *prev_ = nullptr;      /**< The previous object_proxy in the list. */
  object_proxy *next_ = nullptr;      /**< The next object_proxy in the list. */

  void *obj_ = nullptr;         /**< The concrete serializable. */
  deleter deleter_;             /**< The object deleter function */
  namer namer_;                 /**< The object classname function */
  unsigned long oid = 0;        /**< The id of the concrete or expected serializable. */

  unsigned long ref_count_ = 0; /**< The reference counter */
  unsigned long ptr_count_ = 0; /**< The pointer counter */

  object_store *ostore_ = nullptr;    /**< The object_store to which the object_proxy belongs. */
  prototype_node *node_ = nullptr;    /**< The prototype_node containing the type of the serializable. */

  typedef std::set<basic_object_holder *> ptr_set_t; /**< Shortcut to the object_base_ptr_set. */
  ptr_set_t ptr_set_;      /**< This set contains every basic_object_holder pointing to this object_proxy. */
  
  typedef std::list<serializable*> object_list_t;
  typedef std::map<std::string, object_list_t> string_object_list_map_t;
  
  string_object_list_map_t relations;

  std::shared_ptr<basic_identifier> primary_key_ = nullptr;
};
/// @endcond
}

#endif /* OBJECT_PROXY_HPP */
