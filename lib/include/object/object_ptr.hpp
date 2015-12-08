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

#ifndef OBJECT_PTR_HPP
#define OBJECT_PTR_HPP

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

#include "object/object_proxy.hpp"
#include "object/prototype_node.hpp"
#include "object/identifier_resolver.hpp"

#include <memory>
#include <typeinfo>

namespace oos {

namespace detail {
class result_impl;
}

class serializable;
class object_store;
class prototype_node;

/**
 * @class object_base_ptr
 * @brief Base class for the serializable pointer and reference class
 * 
 * This is the base class for the serializable pointer
 * and reference class. The class holds the proxy
 * of the serializable and the id of the serializable.
 */
class OOS_API object_base_ptr
{
protected:
  /**
   * @brief Creates and empty base pointer.
   * 
   * Creates and empty base pointer. The boolean
   * tells the class if the serializable is handled
   * as a reference or an pointer. The difference
   * is that the reference couldn't be deleted
   * from the object_store and the pointer can.
   * 
   * @param is_ref If true the serializable is handled as a reference.
   */
	explicit object_base_ptr(bool is_ref);
  
  /**
   * Copies from another object_base_ptr
   * 
   * @param x the object_base_ptr to copy from.
   */
	object_base_ptr(const object_base_ptr &x);

  /**
   * Assign operator.
   * 
   * @param x The object_base_ptr to assign from.
   */
	object_base_ptr& operator=(const object_base_ptr &x);

  /**
   * @brief Creates an object_base_ptr with a given object_proxy
   * 
   * Creates an object_base_ptr with a given object_proxy. The
   * boolean tells the object_base_ptr if it should be
   * handled as a reference or a pointer.
   * 
   * @param op The object_proxy of the object_base_ptr
   * @param is_ref If true the serializable is handled as a reference.
   */
	object_base_ptr(object_proxy *op, bool is_ref);

  /**
   * Destroys the object_base_ptr
   * and decides wether the underlaying
   * object_proxy is destroyed as well.
   *
   * It is destroyed if it is not inserted
   * into any object_store.
   */
	~object_base_ptr();

public:

  /**
   * Equal to operator for the object_base_ptr
   * 
   * @param x The object_base_ptr to check equality with.
   */
	bool operator==(const object_base_ptr &x) const;

  /**
   * Not equal to operator for the object_base_ptr
   * 
   * @param x The object_base_ptr to check unequality with.
   */
	bool operator!=(const object_base_ptr &x) const;

  /**
   * Resets the object_base_ptr with the given object_proxy.
   * 
   * @param proxy The new object_proxy for the object_base_ptr.
   * @param is_ref Indicates if the given object_proxy is a reference.
   */
	void reset(object_proxy *proxy = 0, bool is_ref = false);

  /**
   * Resets the object_base_ptr with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  void reset(const std::shared_ptr<basic_identifier> &id);

  /**
   * Returns if the serializable is loaded.
   * 
   * @return True if the serializable is loaded.
   */
	bool is_loaded() const;

  /**
   * Returns the serializable id.
   * 
   * @return The id of the serializable.
   */
	unsigned long id() const;

  /**
   * Sets the serializable id. If a proxy
   * is set an exception is thrown.
   * 
   * @param i The new serializable id
   */
  void id(unsigned long i);

  /**
   * Returns the corresponding
   * object_store or nullptr
   */
  object_store* store() const;

  /**
   * Returns the serializable
   * 
   * @return The serializable.
   */
  void* ptr();

  /**
   * Returns the serializable
   *
   * @return The serializable.
   */
	const void* ptr() const;

  /**
   * Returns the serializable
   * 
   * @return The serializable.
   */
  void* lookup_object();

  /**
   * Returns the serializable
   *
   * @return The serializable.
   */
	void* lookup_object() const;

  /**
   * Returns if the serializable is treated as a reference.
   * 
   * @return True if the serializable is treated like a reference.
   */
  virtual bool is_reference() const;

  /**
   * Returns if this object_base_ptr is inside
   * of the object_store. This is important
   * to calculate the reference and pointer
   * counter.
   * 
   * @return True if the object_base_ptr internal
   */
  bool is_internal() const;

  /**
   * Returns the reference count.
   * 
   * @return The reference count.
   */
  unsigned long ref_count() const;

  /**
   * Returns the pointer count.
   * 
   * @return The pointer count.
   */
  unsigned long ptr_count() const;

  /**
   * Returns true if serializable has a primary key
   *
   * @return true if serializable has a primary key
   */
  bool has_primary_key() const;

  /**
   * Gets the primary key of the foreign serializable
   *
   * @return The primary key of the foreign serializable
   */
  std::shared_ptr<basic_identifier> primary_key() const;

  /**
   * Prints the underlaying serializable
   *
   * @param out The output stream to write on.
   * @param x The serializable pointer to print.
   * @return The output stream.
   */
  friend OOS_API std::ostream& operator<<(std::ostream &out, const object_base_ptr &x);

private:
  friend class object_serializer;
  friend class object_proxy;
  friend class object_deleter;
  friend class object_inserter;
  friend class object_store;
  friend class object_container;

  // Todo: change interface to remove friend
  friend class session;
  // Todo: replace private access of proxy with call to reset
  friend class table_reader;

  template < class T, bool TYPE > friend class object_holder;
  template < class T > friend class has_one;

  object_proxy *proxy_ = nullptr;
  bool is_reference_ = false;
  bool is_internal_ = false;
  unsigned long oid_ = 0;
};

/**
 * @class object_holder
 * @brief The object_holder holds a pointer to an serializable.
 * @tparam T The type of the serializable.
 * @tparam TYPE The type of the holder (referencer or pointer).
 *
 * The object_holder holds a pointer to an serializable. The
 * object_holder is a wrapper class for the serializable class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_holder and should be used through the
 * object_holder class.
 * The TYPE template parameter indicates wether the holder acts
 * like a pointer (object can be deleted) or a reference (object
 * can't be directly deleted)
 */
template < class T, bool TYPE >
class object_holder : public object_base_ptr
{
public:
  typedef T object_type;                    /**< Shortcut for serializable type. */
  typedef object_holder<T, TYPE> self;      /**< Shortcut for self class. */

public:
  /**
   * Create an empty object_holder
   */
  object_holder()
    : object_base_ptr(TYPE)
  {}
  /**
   * Copies object_ptr
   *
   * @param x The object_ptr to copy
   */
  object_holder(const self &x)
    : object_base_ptr(x.proxy_, x.is_reference_)
  {}

  /**
   * Copies a object_ref
   *
   * @param x The object_ref to copy
   */
  template < bool TYPE2 >
  object_holder(const object_holder<T, TYPE2> &x)
    : object_base_ptr(x.proxy_, TYPE)
  {}

  /**
   * Create an object_ptr from an serializable
   *
   * @param o The serializable.
   */
  object_holder(T *o)
    : object_base_ptr(new object_proxy(o), TYPE)
  {}

  /**
   * Create an object_ptr from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  explicit object_holder(object_proxy *proxy)
  : object_base_ptr(proxy, TYPE)
  {}

  /**
   * Assign operator.
   *
   * @param x The x serializable to assign from.
   */
  self& operator=(T *x)
  {
//    is_reference_ = TYPE;
    reset(new object_proxy(x), is_reference_);
    return *this;
  }

  /**
   * Return the type string of the serializable
   *
   * @return The type string of the serializable.
   */
  const char* type() const
  {
    return classname_.c_str();
  }

  /**
   * @brief Return the pointer to the serializable of type T.
   *
   * Return the pointer to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The pointer to the serializable of type T.
   */
  T* operator->() const {
    return get();
  }

  /**
   * @brief Return the pointer to the serializable of type T.
   *
   * Return the pointer to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The pointer to the serializable of type T.
   */
  T* operator->() {
    return get();
  }

  /**
   * @brief Return the reference to the serializable of type T.
   *
   * Return the reference to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The reference to the serializable of type T.
   */
  T& operator*() const {
    return *get();
  }

  /**
   * @brief Return the reference to the serializable of type T.
   *
   * Return the reference to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The reference to the serializable of type T.
   */
  T& operator*() {
    return *get();
  }

  /**
   * @brief Return the pointer to the serializable of type T.
   *
   * Return the pointer to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The pointer to the serializable of type T.
   */
  T* get() const {
    return static_cast<T*>(lookup_object());
  }

  /**
   * @brief Return the pointer to the serializable of type T.
   *
   * Return the pointer to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The pointer to the serializable of type T.
   */
  T* get() {
    return static_cast<T*>(lookup_object());
  }

  /**
   * Creates a new identifier, represented by the identifier
   * of the underlaying type.
   *
   * @return A new identifier.
   */
  basic_identifier* create_identifier() const
  {
    return self::identifier_->clone();
  }

private:
  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

template < class T, bool TYPE >
std::string object_holder<T, TYPE>::classname_ = typeid(T).name();

template < class T, bool TYPE >
std::unique_ptr<basic_identifier> object_holder<T, TYPE>::identifier_(identifier_resolver<T>::resolve());

/**
 * Shortcut to object_ptr
 *
 * @tparam T The type of the object_ptr
 */
template < class T > using object_ptr = object_holder<T, false>;

/**
 * Shortcut to object_ref
 *
 * @tparam T The type of the object_ref
 */
template < class T > using object_ref = object_holder<T, true>;

}

#endif /* OBJECT_PTR_HPP */
