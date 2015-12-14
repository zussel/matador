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
class object_holder : public basic_object_holder
{
public:
  typedef T object_type;                    /**< Shortcut for serializable type. */
  typedef object_holder<T, TYPE> self;      /**< Shortcut for self class. */

public:
  /**
   * Create an empty object_holder
   */
  object_holder()
    : basic_object_holder(TYPE)
  {}
  /**
   * Copies object_ptr
   *
   * @param x The object_ptr to copy
   */
  object_holder(const self &x)
    : basic_object_holder(x.proxy_, x.is_reference_)
  {}

  /**
   * Copies a object_ref
   *
   * @param x The object_ref to copy
   */
  template < bool TYPE2 >
  object_holder(const object_holder<T, TYPE2> &x)
    : basic_object_holder(x.proxy_, TYPE)
  {}

  /**
   * Create an object_ptr from an serializable
   *
   * @param o The serializable.
   */
  object_holder(T *o)
    : basic_object_holder(new object_proxy(o), TYPE)
  {}

  /**
   * Create an object_ptr from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  explicit object_holder(object_proxy *proxy)
  : basic_object_holder(proxy, TYPE)
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
