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

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/transaction.hpp"
#include "matador/utils/identifier_resolver.hpp"

#include <memory>
#include <typeinfo>

namespace matador {

/**
 * @brief The object_pointer holds a pointer to an serializable.
 * @tparam T The type of the serializable.
 *
 * The object_pointer holds a pointer to an object. The
 * object_pointer is a wrapper class for the object class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_pointer and should be used through the
 * object_pointer class.
 */
template < class T, object_holder_type OPT >
class object_pointer : public object_holder
{
public:
  typedef T object_type;           /**< Shortcut for serializable type. */
  typedef object_pointer<T, OPT> self;      /**< Shortcut for self class. */

public:
  /**
   * Create an empty object_pointer
   */
  object_pointer()
    : object_holder(OPT)
  {}
  /**
   * Copies object_pointer
   *
   * @param x The object_pointer to copy
   */
  object_pointer(const self &x)
    : object_holder(x.type_, x.proxy_)
  {}

  object_pointer(self &&x)
    : object_holder(std::move(x))
  {}

  /**
   * Create an object_pointer from an object
   *
   * @param o The object.
   */
  object_pointer(T *o)
    : object_holder(OPT, new object_proxy(o))
  {}

  object_pointer(nullptr_t)
    : object_holder(OPT)
  {}

  /**
   * Create an object_pointer from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  explicit object_pointer(object_proxy *proxy)
  : object_holder(OPT, proxy)
  {}

  /**
   * @brief Creates an object_pointer from the given object_pointer object
   *
   * @param x The object_pointer object to created the object_pointer from
   */
  template < object_holder_type OOPT >
  object_pointer(const object_pointer<T, OOPT> &x)
    : object_holder(x)
  {
//    relation_info_ = x.relation_info_;
//    reset(x.proxy_, x.cascade_);
  }

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
   * @brief Copy assignes an object_pointer from the given has_one object
   * @param x The has_one object to created the object_pointer from
   * @return A reference to the created object_pointer
   */
  self& operator=(const self &x)
  {
//    relation_info_ = x.relation_info_;
    reset(x.proxy_, x.cascade_);
    return *this;
  }

  self& operator=(self &&x)
  {
    return static_cast<self&>(object_holder::operator=(std::move(x)));
  }

  /**
   * @brief Copy assignes an object_pointer from the given has_one object
   * @param x The has_one object to created the object_pointer from
   * @return A reference to the created object_pointer
   */
  template < object_holder_type OOPT >
  self& operator=(object_pointer<T, OOPT> &x)
  {
//    relation_info_ = x.relation_info_;
    reset(x.proxy_, x.cascade_);
    return *this;
  }

  self& operator=(nullptr_t)
  {
//    relation_info_.reset();
    clear();
    return *this;
  }
  /**
   * Return the type string of the object
   *
   * @return The type string of the object.
   */
  const char* type() const override
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
  basic_identifier* create_identifier() const override
  {
    return self::identifier_->clone();
  }

private:
  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

/**
 * Shortcut to object_pointer representing a object_ptr
 * relationship
 */
template < class T >
using object_ptr = object_pointer<T, object_holder_type::OBJECT_PTR>;

template < class T, object_holder_type OPT >
std::string object_pointer<T, OPT>::classname_ = typeid(T).name();

template < class T, object_holder_type OPT >
std::unique_ptr<basic_identifier> object_pointer<T, OPT>::identifier_(identifier_resolver<T>::resolve());

}

#endif /* OBJECT_PTR_HPP */
