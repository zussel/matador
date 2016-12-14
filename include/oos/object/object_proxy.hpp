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
  #ifdef oos_object_EXPORTS
    #define OOS_OBJECT_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_OBJECT_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_OBJECT_API
#endif

#include "oos/utils/identifier_resolver.hpp"

#include "oos/object/prototype_node.hpp"

#include <ostream>
#include <set>
#include <list>

#include <map>
#include <memory>

namespace oos {

class object_store;
class object_holder;
class prototype_node;
class basic_identifier;
class transaction;

/**
 * @cond OOS_DEV
 * @class object_proxy
 * @brief A proxy between the object and the object_store
 *
 * Is a proxy between the object and the object store. It stores
 * the pointer to the object and the id. Once a new object
 * is inserted into the 
 */
class OOS_OBJECT_API object_proxy
{
public:

  /**
   * @brief Create an empty object_proxy
   *
   * Create an empty object_proxy
   */
  object_proxy();

  /**
   * Create a new object proxy with primary key
   *
   * @param pk primary key of object
   */
  object_proxy(const std::shared_ptr<basic_identifier> &pk);

  template < class T >
  explicit object_proxy(const std::shared_ptr<basic_identifier> &pk, T *obj, prototype_node *node)
    : obj_(obj)
    , deleter_(&destroy<T>)
    , namer_(&type_id<T>)
    , ostore_(node->tree())
    , node_(node)
    , primary_key_(pk)
  {}

  /**
   * @brief Create an object_proxy for a given object.
   *
   * Create an object_proxy for unknown object
   * with given id.
   *
   * @param o The valid object.
   */
  template < typename T >
  explicit object_proxy(T *o)
    : obj_(o)
    , deleter_(&destroy<T>)
    , namer_(&type_id<T>)
  {
    primary_key_.reset(identifier_resolver<T>::resolve(o));
  }

  /**
   * @brief Create an object_proxy for a given object.
   *
   * Create an object_proxy for unknown object
   * with given id.
   *
   * @param o The valid object.
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
      primary_key_.reset(identifier_resolver<T>::resolve(o));
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
   * Release the object from proxies
   * responsibility. After release the user
   * is responsible for object.
   *
   * @tparam The type of the object
   * @return The released object
   */
  template < class T >
  T* release()
  {
    T* tmp = obj<T>();
    obj_ = nullptr;
    return tmp;
  }

  /**
   * Print the object_proxy to a stream
   *
   * @param os The stream where the object_proxy is printed to
   * @param op The object_proxy to print
   * @return The modified stream.
   */
  friend OOS_OBJECT_API std::ostream& operator <<(std::ostream &os, const object_proxy &op);

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
   * Increment reference counter
   */
  unsigned long operator++();
  unsigned long operator++(int);
  /**
   * Decrement reference counter
   */
  unsigned long operator--();
  unsigned long operator--(int);
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


  unsigned long reference_count() const;

  /**
   * Resets the object of the object_proxy
   * with the given object.
   *
   * @param o The new object for the object_proxy
   */
  template < typename T >
  void reset(T *o, bool resolve_identifier = true)
  {
    reference_counter_ = 0;
    deleter_ = &destroy<T>;
    namer_ = &type_id<T>;
    obj_ = o;
    oid = 0;
    node_ = 0;
    if (obj_ != nullptr && resolve_identifier) {
      primary_key_.reset(identifier_resolver<T>::resolve());
    }
  }


  /**
   * @brief Add an object_holder to the linked list.
   *
   * Each object_holder containing this object_proxy
   * calls this method. So object_proxy knows how many
   * object_holder are dealing with this object.
   *
   * @param ptr The object_holder containing this object_proxy.
   */
  void add(object_holder *ptr);

  /**
   * @brief Remove an object_holder from the linked list.
   *
   * Each destroying ore reseting object_holder
   * containg this object_proxy calls this method.
   * So object_proxy knows how many object_holder
   * are dealing with this object.
   *
   * @param ptr The object_holder containing this object_proxy.
   */
  bool remove(object_holder *ptr);

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
   * Return the id of the object. If no object is
   * set 0 (null) is returned
   *
   * @return 0 (null) or the id of the object.
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
   * object has a primary key
   *
   * @return true If the object has a primary key
   */
  bool has_identifier() const;

  /**
   * Return the primary key. If underlaying object
   * doesn't have a primary key, an uninitialized
   * primary key shared ptr is returned
   *
   * @return The primary key of the underlaying object
   */
  std::shared_ptr<basic_identifier> pk() const;

private:
  transaction current_transaction();
  bool has_transaction() const;

private:
  friend class object_store;
  friend class prototype_node;
  friend class prototype_tree;
  template < class T > friend class result;
  friend class table_reader;
  friend class restore_visitor;
  friend class object_holder;
  template < class T > friend class object_ptr;
  template < class T > friend class has_one;

  typedef void (*deleter)(void*);
  typedef const char* (*namer)();

  template <typename T>
  static void destroy(void* p)
  {
    delete (T*)p;
  }

  template < class T >
  static const char* type_id()
  {
    return typeid(T).name();
  }

  object_proxy *prev_ = nullptr;      /**< The previous object_proxy in the list. */
  object_proxy *next_ = nullptr;      /**< The next object_proxy in the list. */

  void *obj_ = nullptr;         /**< The concrete object. */
  deleter deleter_;             /**< The object deleter function */
  namer namer_;                 /**< The object classname function */
  unsigned long oid = 0;        /**< The id of the concrete or expected object. */


  unsigned long reference_counter_ = 0;

  object_store *ostore_ = nullptr;    /**< The object_store to which the object_proxy belongs. */
  prototype_node *node_ = nullptr;    /**< The prototype_node containing the type of the object. */

  typedef std::set<object_holder *> ptr_set_t; /**< Shortcut to the object_base_ptr_set. */
  ptr_set_t ptr_set_;      /**< This set contains every object_holder pointing to this object_proxy. */
  
  std::shared_ptr<basic_identifier> primary_key_ = nullptr;
};
/// @endcond
}

#endif /* OBJECT_PROXY_HPP */
