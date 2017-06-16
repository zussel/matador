//
// Created by sascha on 12/10/15.
//

#ifndef OBJECT_HOLDER_HPP
#define OBJECT_HOLDER_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
#else
#define MATADOR_OBJECT_API
#endif

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include "matador/object/object_holder_type.hpp"
#include "matador/object/prototype_node.hpp"
#include "matador/object/relation_field_endpoint.hpp"

#include <memory>

namespace matador {

/// @cond MATADOR_DEV

namespace detail {
class object_inserter;
class object_deleter;
class object_proxy_accessor;
template<class T, template <class ...> class C, class Enabled>
class has_many_inserter;
template<class T, template <class ...> class C, class Enabled>
class has_many_deleter;
class basic_relation_endpoint;
template < class Value >
class relation_endpoint_value_inserter;
template < class Value >
class relation_endpoint_value_remover;
}

/// @endcond

class basic_identifier;
class object_proxy;
class object_store;

/**
 * @class object_holder
 * @brief Base class for the object pointer and reference class
 *
 * This is the base class for the object pointer
 * and reference class. The class holds the proxy
 * of the object and the id of the object.
 */
class MATADOR_OBJECT_API object_holder : public identifiable_holder
{
protected:
  /**
   * @brief Creates and empty base pointer.
   * 
   * Creates and empty base pointer. The boolean
   * tells the class if the object is handled
   * as a reference or an pointer. The difference
   * is that the reference couldn't be deleted
   * from the object_store and the pointer can.
   *
   * @param holder_type Type of the object holder
   */
  explicit object_holder(object_holder_type holder_type);

  /**
   * Copies from another object_holder
   * 
   * @param x the object_holder to copy from.
   */
  object_holder(const object_holder &x);

  /**
   * Assign operator.
   * 
   * @param x The object_holder to assign from.
   */
  object_holder & operator=(const object_holder &x);

  /**
   * @brief Creates an object_holder with a given object_proxy
   * 
   * Creates an object_holder with a given object_proxy. The
   * boolean tells the object_holder if it should be
   * handled as an internal.
   * 
   * @param holder_type Type of the object holder
   * @param op The object_proxy of the object_holder
   */
  object_holder(object_holder_type holder_type, object_proxy *op);

  /**
   * Destroys the object_holder
   * and decides wether the underlaying
   * object_proxy is destroyed as well.
   *
   * It is destroyed if it is not inserted
   * into any object_store.
   */
  virtual ~object_holder();

public:

  /**
   * Equal to operator for the object_holder
   * 
   * @param x The object_holder to check equality with.
   */
  bool operator==(const object_holder &x) const;

  /**
   * Not equal to operator for the object_holder
   * 
   * @param x The object_holder to check unequality with.
   */
  bool operator!=(const object_holder &x) const;

  /**
   * Resets the object_holder with the given object_proxy.
   * 
   * @param proxy The new object_proxy for the object_holder.
   * @param cascade Sets the cascadable actions for the proxy.
   */
  void reset(object_proxy *proxy, cascade_type cascade);
  void reset(object_holder &holder);

  /**
   * Resets the object_holder with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  void reset(const std::shared_ptr<basic_identifier> &id);

  /**
   * Clears the currently set object
   */
  void clear();

  /**
   * Returns true if object_holder doesn't
   * holds an object
   *
   * @return True if object_holder doesn't holds an object
   */
  bool empty() const;

  /**
   * Returns if the object is loaded.
   * 
   * @return True if the object is loaded.
   */
  bool is_loaded() const;

  /**
   * Returns the object id.
   * 
   * @return The id of the object.
   */
  unsigned long id() const;

  /**
   * Sets the object id. If a proxy
   * is set an exception is thrown.
   * 
   * @param i The new object id
   */
  void id(unsigned long i);

  /**
   * Returns the corresponding
   * object_store or nullptr
   */
  object_store* store() const;

  /**
   * Returns the raw object pointer
   * 
   * @return The raw object pointer.
   */
  void* ptr();

  /**
   * Returns the raw object pointer
   *
   * @return The raw object pointer.
   */
  const void* ptr() const;

  /**
   * Returns the object pointer
   * 
   * @return The object pointer.
   */
  void* lookup_object();

  /**
   * Returns the object pointer
   *
   * @return The object pointer.
   */
  void* lookup_object() const;

  /**
   * Returns if this object_holder is inside
   * of the object_store. This is important
   * to calculate the reference and pointer
   * counter.
   * 
   * @return True if the object_holder internal
   */
  bool is_internal() const;

  /**
   * Returns true if holder represents a belongs_to
   *
   * @return True if holder represents a belongs_to
   */
  bool is_belongs_to() const;

  /**
   * Returns true if holder represents a has_one
   *
   * @return True if holder represents a has_one
   */
  bool is_has_one() const;

  /**
   * Returns true if holder represents a object_ptr
   *
   * @return True if holder represents a object_ptr
   */
  bool is_object_ptr() const;

  /**
   * Returns true if the underlying object
   * is inserted in an object_store
   *
   * @return True if object is inserted.
   */
  bool is_inserted() const;

  /**
   * Returns true if object has a primary key
   *
   * @return true if object has a primary key
   */
  bool has_primary_key() const;

  /**
   * Gets the primary key of the foreign object
   *
   * @return The primary key of the foreign object
   */
  virtual std::shared_ptr<basic_identifier> primary_key() const;

  /**
   * Returns the current reference count
   *
   * @return The current reference count
   */
  unsigned long reference_count() const;

  /**
   * Return the type string of the object
   *
   * @return The type string of the object.
   */
  virtual const char* type() const = 0;

  /**
   * Return the object holder type.
   *
   * @return The object holder type.
   */
  object_holder_type holder_type() const;

  /**
   * Prints the underlaying object
   *
   * @param out The output stream to write on.
   * @param x The object pointer to print.
   * @return The output stream.
   */
  friend MATADOR_OBJECT_API std::ostream& operator<<(std::ostream &out, const object_holder &x);

private:
  void reset(object_proxy *proxy, cascade_type cascade, bool notify_foreign_relation);

private:
  friend class object_serializer;
  friend class object_proxy;
  friend class detail::object_deleter;
  friend class detail::object_inserter;
  friend class object_store;
  friend class object_container;
  friend class detail::object_proxy_accessor;
  template<class T, template <class ...> class C, class Enabled >
  friend class detail::has_many_inserter;
  template<class T, template <class ...> class C, class Enabled >
  friend class detail::has_many_deleter;
  template < class L, class R >
  friend class has_many_to_many_item;
  template < class T >
  friend class detail::relation_endpoint_value_inserter;
  template < class T >
  friend class detail::relation_endpoint_value_remover;

  // Todo: change interface to remove friend
  friend class session;
  // Todo: replace private access of proxy with call to reset
  friend class table_reader;

  template < class T, object_holder_type OPT >
  friend class object_pointer;
  template < class T, class Enable >
  friend class has_many_item_holder;
  friend class detail::basic_relation_endpoint;

  object_proxy *proxy_ = nullptr;
  object_proxy *owner_ = nullptr; // only set if holder type is BELONGS_TO or HAS_MANY
  cascade_type cascade_ = cascade_type::NONE;
  object_holder_type type_;
  bool is_inserted_ = false;
  unsigned long oid_ = 0;

  std::shared_ptr<detail::basic_relation_endpoint> relation_info_;
};

}

#endif //OBJECT_HOLDER_HPP
