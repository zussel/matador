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

#include "object/object_proxy.hpp"
#include "object/prototype_node.hpp"
#include "object/basic_object_holder.hpp"
#include "object/identifier_resolver.hpp"
#include "object/has_one.hpp"

#include <memory>
#include <typeinfo>

namespace oos {

namespace detail {
class result_impl;
}

class object_store;
class prototype_node;

template < class T > class object_ptr;

template < class T >
class has_one : public basic_object_holder
{
public:
  typedef has_one<T> self;      /**< Shortcut for self class. */

public:
  has_one()
    : basic_object_holder(true)
  {}
  has_one(T *o)
    : basic_object_holder(true, new object_proxy(o))
  {}
  has_one(object_proxy *proxy)
    : basic_object_holder(true, proxy)
  {}

  has_one(const object_ptr <T> &x);

  has_one& operator=(object_ptr <T> &x);

  T* operator->()
  {
    return static_cast<T*>(proxy_->obj());
  }

  T* get()
  {
    return static_cast<T*>(proxy_->obj());
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
  friend class object_deleter;

private:
  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

template < class T >
std::string has_one<T>::classname_ = typeid(T).name();

template < class T >
std::unique_ptr<basic_identifier> has_one<T>::identifier_(identifier_resolver<T>::resolve());


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
template < class T >
class object_ptr : public basic_object_holder
{
public:
  typedef T object_type;                    /**< Shortcut for serializable type. */
  typedef object_ptr<T> self;      /**< Shortcut for self class. */

public:
  /**
   * Create an empty object_holder
   */
  object_ptr()
    : basic_object_holder(false)
  {}
  /**
   * Copies object_ptr
   *
   * @param x The object_ptr to copy
   */
  object_ptr(const self &x)
    : basic_object_holder(x.is_internal_, x.proxy_, x.cascade_)
  {}

  /**
   * Create an object_ptr from an serializable
   *
   * @param o The serializable.
   */
  object_ptr(T *o)
    : basic_object_holder(false, new object_proxy(o))
  {}

  /**
   * Create an object_ptr from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  explicit object_ptr(object_proxy *proxy)
  : basic_object_holder(false, proxy)
  {}

  object_ptr(const has_one<T> &x)
    : basic_object_holder(false, x.proxy_, x.cascade_)
  {}

  /**
   * Assign operator.
   *
   * @param x The x serializable to assign from.
   */
  self& operator=(T *x)
  {
    reset(*x);
//    reset(new object_proxy(x), is_reference_);
    return *this;
  }

  self& operator=(has_one<T> &x)
  {
    reset(x);
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

template < class T >
std::string object_ptr<T>::classname_ = typeid(T).name();

template < class T >
std::unique_ptr<basic_identifier> object_ptr<T>::identifier_(identifier_resolver<T>::resolve());


template < class T >
has_one<T>::has_one(const object_ptr<T> &x)
  : basic_object_holder(true, x.proxy_, x.cascade_)
{}

template < class T >
has_one<T>& has_one<T>::operator=(object_ptr <T> &x)
{
  reset(x);
  return *this;
}

}

#endif /* OBJECT_PTR_HPP */
