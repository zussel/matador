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

#ifdef WIN32
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

#include <memory>
#include <iostream>
#include <typeinfo>

namespace oos {

class object;

/**
 * @class object_base_ptr
 * @brief Base class for the object pointer and reference class
 * 
 * This is the base class for the object pointer
 * and reference class. The class holds the proxy
 * of the object and the id of the object.
 */
class OOS_API object_base_ptr
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
   * @param is_ref If true the object is handled as a reference.
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
   * @brief Creates an object_base_ptr with a given object
   * 
   * Creates an object_base_ptr with a given object. The
   * boolean tells the object_base_ptr if it should be
   * handled as a reference or a pointer.
   * 
   * @param o The object of the object_base_ptr
   * @param is_ref If true the object is handled as a reference.
   */
	object_base_ptr(object* o, bool is_ref);
  
  /**
   * @brief Creates an object_base_ptr with a given object_proxy
   * 
   * Creates an object_base_ptr with a given object_proxy. The
   * boolean tells the object_base_ptr if it should be
   * handled as a reference or a pointer.
   * 
   * @param op The object_proxy of the object_base_ptr
   * @param is_ref If true the object is handled as a reference.
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
   * Returns the type string of the object
   * 
   * @return The type string of the object.
   */
  virtual const char* type() const = 0;

//  const char* classname() const;

  /**
   * Resets the object_base_ptr with the given object.
   * 
   * @param o The new object for the object_base_ptr.
   */
	void reset(const object *o = 0);

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
	long id() const;

  /**
   * Sets the object id. If a proxy
   * is set an exception is thrown.
   * 
   * @param i The new object id
   */
  void id(long i);

  /**
   * Returns the object
   * 
   * @return The object.
   */
	object* ptr() const;

  /**
   * Returns the object
   * 
   * @return The object.
   */
	object* lookup_object() const;

  /**
   * Returns if the object is treated as a reference.
   * 
   * @return True if the object is treated like a reference.
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
   * Prints the underlaying object
   *
   * @param out The output stream to write on.
   * @param x The object pointer to print.
   * @return The output stream.
   */
  friend OOS_API std::ostream& operator<<(std::ostream &out, const object_base_ptr &x);

private:
	friend class object_reader;
	friend class object_writer;
  friend class object_creator;
  friend class object_serializer;
  friend struct object_proxy;

  template < class T > friend class object_ref;
  template < class T > friend class object_ptr;

	long id_;
  object_proxy *proxy_;
  bool is_reference_;
  bool is_internal_;
};

/// @cond OOS_DEV

template < class T >
class object_ref;

/// @endcond OOS_DEV

/**
 * @class object_ptr
 * @brief The object_ptr holds a pointer to an object.
 * @tparam T The type of the object.
 * 
 * The object_ptr holds a pointer to an object. The
 * object_ptr is a wrapper class for the object class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_ptr and should be used through the
 * object_ptr class.
 */
template < class T >
class object_ptr : public object_base_ptr
{
public:
  typedef T object_type; /**< Shortcut for object type. */

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
   * Assign an object_ptr
   * 
   * @param x The object_ptr to assign from
   * @return The assign object_ptr
   */
	object_ptr& operator=(const object_ptr &x)
  {
    object_base_ptr::operator=(x);
    return *this;
  }

  /**
   * Create an object_ptr from an object
   * 
   * @param o The object.
   */
	/*explicit */object_ptr(object* o)
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
   * Return the type string of the object
   * 
   * @return The type string of the object.
   */
  virtual const char* type() const
  {
    return typeid(T).name();
  }

  /**
   * @brief Return the pointer to the object of type T.
   *
   * Return the pointer to the object of type T. If there
   * isn't a valid object 0 (null) is returned.
   *
   * @return The pointer to the object of type T.
   */
	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}

  /**
   * @brief Return the reference to the object of type T.
   *
   * Return the reference to the object of type T. If there
   * isn't a valid object 0 (null) is returned.
   *
   * @return The reference to the object of type T.
   */
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
    } else {
      return *(T*)0;
    }
	}

  /**
   * @brief Return the pointer to the object of type T.
   *
   * Return the pointer to the object of type T. If there
   * isn't a valid object 0 (null) is returned.
   *
   * @return The pointer to the object of type T.
   */
  T* get() const {
    if (proxy_) {
      T* t = dynamic_cast<T*>(lookup_object());
      return t;
    }
    return NULL;
  }
};

/**
 * @class object_ref
 * @brief The object_ref holds a pointer to an object.
 * @tparam T The type of the object.
 * 
 * The object_ref holds a pointer to an object. The
 * object_ref is a wrapper class for the object class
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
  typedef T object_type;  /**< Shortcut for object type. */
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
   * Assign an object_ref
   * 
   * @param x The object_ref to assign from
   * @return The assign object_ref
   */
	object_ref& operator=(const object_ref &x)
  {
    object_base_ptr::operator=(x);
    return *this;
  }

  /**
   * @brief Check on equal.
   *
   * Check if this object_ref is equal to
   * the given one.
   *
   * @param x The object_ref to compare with.
   * @return True if the object_ref are equal.
   */
  bool operator==(const object_ref &x) const
  {
    return object_base_ptr::operator ==(x);
  }

  /**
   * Create an object_ref from an object
   * 
   * @param o The object.
   */
  explicit object_ref(object* o) : object_base_ptr(o, true) {}

  /**
   * Create an object_ref from an object_proxy
   * 
   * @param proxy The object.
   */
  explicit object_ref(object_proxy *proxy) : object_base_ptr(proxy, true) {}

  /**
   * Return the type string of the object
   * 
   * @return The type string of the object.
   */
  virtual const char* type() const
  {
    return typeid(T).name();
  }

  /**
   * @brief Return the pointer to the object of type T.
   *
   * Return the pointer to the object of type T. If there
   * isn't a valid object 0 (null) is returned.
   *
   * @return The pointer to the object of type T.
   */
	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}

  /**
   * @brief Return the reference to the object of type T.
   *
   * Return the reference to the object of type T. If there
   * isn't a valid object 0 (null) is returned.
   *
   * @return The reference to the object of type T.
   */
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}

  /**
   * @brief Return the pointer to the object of type T.
   *
   * Return the pointer to the object of type T. If there
   * isn't a valid object 0 (null) is returned.
   *
   * @return The pointer to the object of type T.
   */
	T* get() const {
		if (proxy_) {
      return dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
};

}

#endif /* OBJECT_PTR_HPP */
