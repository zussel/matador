#ifndef OBJECT_PTR_HPP
#define OBJECT_PTR_HPP

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/utils/identifier_resolver.hpp"

#include <memory>
#include <typeinfo>

namespace matador {

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
  object_ptr() = default;
  /**
   * Copies object_ptr
   *
   * @param x The object_ptr to copy
   */
  object_ptr(const self &x)
    : object_holder(x.proxy_)
  {}

  /**
   * Move constructor
   * 
   * @param x object_ptr to move
   */
  object_ptr(self &&x) noexcept
    : object_holder(std::move(x))
  {}

  /**
   * Create an object_ptr from an object
   *
   * @param o The object.
   */
  object_ptr(T *o)
    : object_holder(new object_proxy(o))
  {}

  /**
   * Initializes the object_ptr
   * with nullptr
   */ 
  object_ptr(std::nullptr_t)
  {}

  /**
   * Create an object_ptr from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  explicit object_ptr(object_proxy *proxy)
  : object_holder(proxy)
  {}

  /**
   * Assign operator.
   *
   * @param x The x serializable to assign from.
   */
  object_ptr& operator=(T *x)
  {
    reset(new object_proxy(x), cascade_);
    return *this;
  }

  /**
   * @brief Copy assignes an object_ptr from the given has_one object
   * @param x The has_one object to created the object_ptr from
   * @return A reference to the created object_ptr
   */
  object_ptr& operator=(const self &x)
  {
    reset(x.proxy_, x.cascade_);
    return *this;
  }

  /**
   * Move assignment constructor
   * 
   * @param x object_ptr to be moved
   */
  object_ptr& operator=(self &&x) noexcept
  {
    object_holder::operator=(std::move(x));
    return *this;
  }

  /**
   * Assigns nullptr to the object_ptr
   * 
   * @return Cleared reference to object_ptr
   */
  object_ptr& operator=(std::nullptr_t)
  {
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

  /**
   * @brief Return the pointer to the serializable of type T.
   *
   * Return the pointer to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The pointer to the serializable of type T.
   */
  const T* operator->() const {
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
  const T* get() const {
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
   * The modify method allows to modify the underlying
   * object. It ensures that this object is marked
   * as modified in the object_store and all registered
   * observers are notified
   *
   * @return Pointer to the underlying object
   */
  T* modify();

  /**
   * @brief Return the reference to the serializable of type T.
   *
   * Return the reference to the serializable of type T. If there
   * isn't a valid serializable 0 (null) is returned.
   *
   * @return The reference to the serializable of type T.
   */
  const T& operator*() const {
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
   * Creates a new identifier, represented by the identifier
   * of the underlying type.
   *
   * @return A new identifier.
   */
  identifier create_identifier() const override
  {
    return self::identifier_;
  }

private:
  static std::string classname_;
  static identifier identifier_;
};

template < class T >
T* object_ptr<T>::modify() {
  if (proxy_ && proxy_->obj()) {
    proxy_->mark_modified();

    return proxy_->obj<T>();
  } else {
    return nullptr;
  }
}

template < class T >
std::string object_ptr<T>::classname_ = typeid(T).name();

template < class T >
identifier object_ptr<T>::identifier_(identifier_resolver<T>::create());

}

#endif /* OBJECT_PTR_HPP */
