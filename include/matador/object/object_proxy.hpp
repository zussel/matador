#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

#ifdef _MSC_VER
  #ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define MATADOR_OBJECT_API
#endif

#include "matador/utils/identifier_resolver.hpp"

#include "matador/object/object_holder_type.hpp"
#include "matador/object/object_type_registry_entry_base.hpp"

#include <ostream>
#include <set>
#include <list>
#include <functional>
#include <map>
#include <memory>

namespace matador {

class object_store;
class object_holder;
class prototype_node;
class basic_identifier;
class transaction;
class update_action;
class object_serializer;
class object_deserializer;

namespace detail {
class basic_relation_data;

template<typename T> struct identity { typedef T type; };

}

/**
 * @cond MATADOR_DEV
 * @class object_proxy
 * @brief A proxy between the object and the object_store
 *
 * Is a proxy between the object and the object store. It stores
 * the pointer to the object and the id. Once a new object
 * is inserted into the 
 */
class MATADOR_OBJECT_API object_proxy
{
public:

  /**
   * @brief Create an empty object_proxy
   *
   * Create an empty object_proxy
   */
  object_proxy() = default;

  /**
   * Create a new object proxy with primary key
   *
   * @param pk primary key of object
   */
  explicit object_proxy(basic_identifier *pk);

  template < class T >
  object_proxy(basic_identifier *pk, const std::shared_ptr<detail::object_type_registry_entry_base> &object_type_entry, detail::identity<T>)
    : object_type_entry_(object_type_entry)
    , deleter_(&destroy<T>)
    , creator_(&create<T>)
    , name_(&type_id<T>)
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
    , creator_(&create<T>)
    , name_(&type_id<T>)
  {
    primary_key_ = identifier_resolver<T>::resolve(o);
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
  object_proxy(T *o, unsigned long id, const std::shared_ptr<detail::object_type_registry_entry_base> &object_type_entry)
    : obj_(o)
    , object_type_entry_(object_type_entry)
    , deleter_(&destroy<T>)
    , creator_(&create<T>)
    , name_(&type_id<T>)
    , oid(id)
  {
    if (obj_ != nullptr) {
      primary_key_ = identifier_resolver<T>::resolve(o);
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
   * Return the underlying object
   *
   * @tparam The type of the object
   * @return The underlying object
   */
  template < typename T = void >
  T* obj()
  {
    return static_cast<T*>(obj_);
  }

  /**
   * Return the underlying object
   *
   * @tparam The type of the object
   * @return The underlying object
   */
  template < typename T = void >
  const T* obj() const
  {
    return static_cast<const T*>(obj_);
  }

  /**
   * Return the underlying object store
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
  friend MATADOR_OBJECT_API std::ostream& operator <<(std::ostream &os, const object_proxy &op);

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
  void reset(T *o, bool resolve_identifier = true, bool keep_ref_count = false)
  {
    if (!keep_ref_count) {
      reference_counter_ = 0;
    }
    obj_ = o;
    oid = 0;
    if (obj_ != nullptr && resolve_identifier) {
      primary_key_ = identifier_resolver<T>::resolve(o);
    }
  }

  void restore(byte_buffer &buffer, object_deserializer &deserializer);

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
   * Each destroying resets object_holder
   * containing this object_proxy calls this method.
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
   * Return the primary key. If underlying object
   * doesn't have a primary key, an uninitialized
   * primary key is returned
   *
   * @return The primary key of the underlying object
   */
  basic_identifier* pk() const;

  void pk(basic_identifier *id);

  void backup(byte_buffer &buffer, object_serializer &serializer);

  void create_object();

private:
  friend class object_store;
  friend class prototype_node;
  template < class T > friend class result;
  friend class object_holder;
  template < class T, object_holder_type OHT > friend class object_pointer;
  friend class detail::basic_relation_data;
  using delete_func = std::function<void(void*)>;
  using create_func = std::function<void(object_proxy*)>;
  using name_func = std::function<const char*()>;


  template <typename T>
  static void destroy(void* p)
  {
    delete (T*)p;
  }

  template<typename T>
  static void create(object_proxy *proxy)
  {
    proxy->template reset(new T);
  }

  template < class T >
  static const char* type_id()
  {
    return typeid(T).name();
  }

  object_proxy *prev_ = nullptr;      /**< The previous object_proxy in the list. */
  object_proxy *next_ = nullptr;      /**< The next object_proxy in the list. */

  void *obj_ = nullptr;                       /**< The concrete object. */

  std::shared_ptr<detail::object_type_registry_entry_base> object_type_entry_;

  delete_func deleter_ = nullptr;             /**< The object deleter function */
  create_func creator_ = nullptr;
  name_func name_ = nullptr;                 /**< The object classname function */

  unsigned long oid = 0;                      /**< The id of the concrete or expected object. */

  unsigned long reference_counter_ = 0;

  typedef std::set<object_holder *> ptr_set_t; /**< Shortcut to the object_holder set. */
  ptr_set_t ptr_set_;      /**< This set contains every object_holder pointing to this object_proxy. */
  
  basic_identifier *primary_key_ = nullptr;
};
/// @endcond
}

#endif /* OBJECT_PROXY_HPP */
