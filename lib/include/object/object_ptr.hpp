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
   * @brief Creates an object_base_ptr with a given serializable
   * 
   * Creates an object_base_ptr with a given serializable. The
   * boolean tells the object_base_ptr if it should be
   * handled as a reference or a pointer.
   * 
   * @param o The serializable of the object_base_ptr
   * @param is_ref If true the serializable is handled as a reference.
   */
	object_base_ptr(serializable *o, bool is_ref);
  
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

	virtual ~object_base_ptr();

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
   * Returns the type string of the serializable
   * 
   * @return The type string of the serializable.
   */
  virtual const char* type() const = 0;

//  const char* classname() const;

  /**
   * Resets the object_base_ptr with the given serializable.
   * 
   * @param o The new serializable for the object_base_ptr.
   */
	void reset(object_proxy *proxy = 0, bool is_ref = false);

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
  serializable* ptr();

  /**
   * Returns the serializable
   *
   * @return The serializable.
   */
	const serializable* ptr() const;

  /**
   * Returns the serializable
   * 
   * @return The serializable.
   */
  serializable* lookup_object();
	serializable* lookup_object() const;

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

  virtual basic_identifier* create_identifier() const = 0;

  /**
   * Prints the underlaying serializable
   *
   * @param out The output stream to write on.
   * @param x The serializable pointer to print.
   * @return The output stream.
   */
  friend OOS_API std::ostream& operator<<(std::ostream &out, const object_base_ptr &x);

private:
  friend class object_creator;
  friend class object_serializer;
  friend class object_proxy;
  friend class object_deleter;
  friend class object_store;
  friend class object_container;

  // Todo: change interface to remove friend
  friend class session;
  // Todo: replace private access of proxy with call to reset
  friend class table_reader;

  template < class T > friend class object_ref;
  template < class T > friend class object_ptr;

  object_proxy *proxy_ = nullptr;
  std::shared_ptr<oos::basic_identifier> identifier_;
  bool is_reference_ = false;
  bool is_internal_ = false;
  unsigned long oid_ = 0;
};

/// @cond OOS_DEV

template < class T >
class object_ref;

/// @endcond OOS_DEV

/**
 * @class object_ptr
 * @brief The object_ptr holds a pointer to an serializable.
 * @tparam T The type of the serializable.
 * 
 * The object_ptr holds a pointer to an serializable. The
 * object_ptr is a wrapper class for the serializable class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_ptr and should be used through the
 * object_ptr class.
 */
template < class T >
class object_ptr : public object_base_ptr
{
public:
  typedef T object_type; /**< Shortcut for serializable type. */

public:
  /**
   * Create an empty object_ptr
   */
	object_ptr()
    : object_base_ptr(false)
  {}
  
  /**
   * Copies object_ptr
   * 
   * @param x The object_ptr to copy
   */
  object_ptr(const object_ptr &x)
    : object_base_ptr(x.proxy_, false)
  {}

  /**
   * Copies a object_ref
   * 
   * @param x The object_ref to copy
   */
  object_ptr(const object_ref<T> &x)
    : object_base_ptr(x.proxy_, false)
  {}

  /**
   * Create an object_ptr from an serializable
   * 
   * @param o The serializable.
   */
	/*explicit */object_ptr(serializable * o)
    : object_base_ptr(o, false)
  {}

  /**
   * Create an object_ptr from an object_proxy
   * 
   * @param proxy The object_proxy.
   */
  explicit object_ptr(object_proxy *proxy)
    : object_base_ptr(proxy, false)
  {}

  /**
   * Return the type string of the serializable
   * 
   * @return The type string of the serializable.
   */
  virtual const char* type() const
  {
    return classname_.c_str();
  }

  /**
   * Assign operator.
   *
   * @param x The x serializable to assign from.
   */
  object_ptr<T>& operator=(serializable *x)
  {
    is_reference_ = false;
    reset(new object_proxy(x, nullptr), is_reference());
    return *this;
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
  T* get() {
      return static_cast<T*>(lookup_object());
  }

  virtual basic_identifier* create_identifier() const
  {
    return object_ptr<T>::identifier_->clone();
  }

private:
  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

template < class T >
std::string object_ptr<T>::classname_ = typeid(T).name();

template < class T >
std::unique_ptr<basic_identifier> object_ptr<T>::identifier_(identifier_resolver::resolve<T>());

/**
 * @class object_ref
 * @brief The object_ref holds a pointer to an serializable.
 * @tparam T The type of the serializable.
 * 
 * The object_ref holds a pointer to an serializable. The
 * object_ref is a wrapper class for the serializable class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_ref and should be used through the
 * object_ref class.
 * A object_ref can't be deleted from the object_store.
 */
template < class T >
class object_ref : public object_base_ptr
{
public:
  typedef T object_type;  /**< Shortcut for serializable type. */
public:
  /**
   * Create an empty object_ref
   */
	object_ref()
    : object_base_ptr(true)
  {}
  
  /**
   * Copies object_ref
   * 
   * @param x The object_ref to copy
   */
  object_ref(const object_ref &x)
    : object_base_ptr(x.proxy_, true)
  {}
  
  /**
   * Copies object_ptr
   * 
   * @param x The object_ptr to copy
   */
  object_ref(const object_ptr<T> &x)
    : object_base_ptr(x.proxy_, true)
  {}

  /**
   * Create an object_ref from an serializable
   * 
   * @param o The serializable.
   */
  explicit object_ref(serializable * o) : object_base_ptr(o, true) {}

  /**
   * Create an object_ref from an object_proxy
   * 
   * @param proxy The serializable.
   */
  explicit object_ref(object_proxy *proxy) : object_base_ptr(proxy, true) {}

  /**
   * Return the type string of the serializable
   * 
   * @return The type string of the serializable.
   */
  virtual const char* type() const
  {
    return classname_.c_str();
  }

  /**
   * Assign operator.
   *
   * @param x The x serializable to assign from.
   */
  object_ref<T>& operator=(serializable *x)
  {
    is_reference_ = true;
    reset(new object_proxy(x, nullptr), is_reference());
    return *this;
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
	T* get() {
    return static_cast<T*>(lookup_object());
	}

  virtual basic_identifier* create_identifier() const
  {
    return object_ref<T>::identifier_->clone();
  }
private:
  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

template < class T >
std::string object_ref<T>::classname_ = typeid(T).name();

template < class T >
std::unique_ptr<basic_identifier> object_ref<T>::identifier_(identifier_resolver::resolve<T>());

}

#endif /* OBJECT_PTR_HPP */
