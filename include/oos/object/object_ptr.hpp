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

#include "oos/object/object_proxy.hpp"
#include "oos/object/object_holder.hpp"
#include "oos/object/transaction.hpp"
#include "oos/utils/identifier_resolver.hpp"

#include <memory>
#include <typeinfo>

namespace oos {

template < class T > class has_one;
template < class T > class belongs_to;

/**
 * @brief The object_ptr holds a pointer to an serializable.
 * @tparam T The type of the serializable.
 *
 * The object_ptr holds a pointer to an object. The
 * object_ptr is a wrapper class for the object class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_ptr and should be used through the
 * object_ptr class.
 */
template < class T >
class object_ptr : public object_holder
{
public:
  typedef T object_type;           /**< Shortcut for serializable type. */
  typedef object_ptr<T> self;      /**< Shortcut for self class. */

public:
  /**
   * Create an empty object_ptr
   */
  object_ptr()
    : object_holder(false)
  {}
  /**
   * Copies object_ptr
   *
   * @param x The object_ptr to copy
   */
  object_ptr(const self &x)
    : object_holder(x.is_internal_, x.proxy_)
  {}

  /**
   * Create an object_ptr from an object
   *
   * @param o The object.
   */
  object_ptr(T *o)
    : object_holder(false, new object_proxy(o))
  {}

  /**
   * Create an object_ptr from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  explicit object_ptr(object_proxy *proxy)
  : object_holder(false, proxy)
  {}

  /**
   * @brief Creates an object_ptr from the given has_one object
   * @param x The has_one object to created the object_ptr from
   */
  object_ptr(const has_one<T> &x);

  /**
   * @brief Creates an object_ptr from the given has_one object
   * @param x The has_one object to created the object_ptr from
   */
  object_ptr(const belongs_to<T> &);

  /**
   * Assign operator.
   *
   * @param x The x serializable to assign from.
   */
  self& operator=(T *x)
  {
    reset(new object_proxy(x), cascade_type::NONE);
    return *this;
  }

  /**
   * @brief Copy assignes an object_ptr from the given has_one object
   * @param x The has_one object to created the object_ptr from
   * @return A reference to the created object_ptr
   */
  self& operator=(has_one<T> &x);

  /**
   * @brief Copy assignes an object_ptr from the given belongs_to object
   * @param x The belongs_to object to created the object_ptr from
   * @return A reference to the created object_ptr
   */
  self& operator=(belongs_to<T> &x);

  /**
   * Return the type string of the object
   *
   * @return The type string of the object.
   */
  const char* type() const
  {
    return classname_.c_str();
  }

  //@{
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

  T* get() const {
    return static_cast<T*>(lookup_object());
  }
  T* get() {
    if (proxy_ && proxy_->obj()) {
      if (proxy_->ostore_ && proxy_->has_transaction()) {
        proxy_->current_transaction().on_update<T>(proxy_);
      }
      return (T*)proxy_->obj();
    } else {
      return nullptr;
    }

  }
  //@}

  //@{
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
  //@}

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
  friend class has_one<T>;
  friend class belongs_to<T>;

  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

template < class T >
std::string object_ptr<T>::classname_ = typeid(T).name();

template < class T >
std::unique_ptr<basic_identifier> object_ptr<T>::identifier_(identifier_resolver<T>::resolve());

}

#endif /* OBJECT_PTR_HPP */
