//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_ONE_HPP
#define OOS_HAS_ONE_HPP

#include "oos/object/object_ptr.hpp"

namespace oos {
namespace detail {
class object_inserter;
class object_deleter;
}

/**
 * @brief The has_one holds a pointer to an serializable.
 * @tparam T The type of the serializable.
 *
 * The has_one holds a pointer to an object. The
 * has_one is a wrapper class for the object class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a has_one and should be used through the
 * has_one class.
 */
template < class T >
class has_one : public identifiable_holder
{
public:
  typedef T object_type;    /**< Shortcut for template type. */
  typedef has_one<T> self; /**< Shortcut for self class. */

public:
  /**
   * Create an empty has_one
   */
  has_one() {}

  /**
   * Create an hs_one from an object
   *
   * @param obj The object.
   */
  has_one(object_type *obj) : optr_(obj) {}

  /**
   * Create an has_one from an object_proxy
   *
   * @param proxy The object_proxy.
   */
  has_one(object_proxy *proxy) : optr_(proxy) {}

  /**
   * Copies has_one from object_ptr
   *
   * @param optr The object_ptr to copy
   */
  has_one(const object_ptr<T> &optr) : optr_(optr) {}

  /**
   * Copies has_one from has_one
   *
   * @param x The has_one to copy
   */
  has_one(const self &x) : optr_(x.optr_) {}

  /**
   * Assigns has_one from has_one
   *
   * @param x The has_one to assign
   * @return Reference to assigned has_one
   */
  has_one& operator=(const self &x)
  {
    reset(x.optr_);
    return *this;
  }

  /**
   * Assigns has_one from object_ptr
   *
   * @param optr The object_ptr to assign
   * @return Reference to assigned has_one
   */
  has_one& operator=(const object_ptr<T> &optr)
  {
    reset(optr);
    return *this;
  }
  
  /**
   * Resets this with given object_ptr
   * 
   * @param optr object_ptr to reset 
   */
  void reset(const object_ptr<T> &optr)
  {
    reset(optr.proxy_, optr.cascade_);
  }
  
  /**
   * Resets has_one from given object_proxy and
   * cascade_type.
   * 
   * @param proxy object_proxy to be set 
   * @param cascade cascade_type to be set
   */
  void reset(object_proxy *proxy, cascade_type cascade)
  {
    optr_.reset(proxy, cascade);
  }

  void reset(const std::shared_ptr<basic_identifier> &id) override
  {
    optr_.reset(id);
  }

  //@{
  /**
   * @brief Return the pointer to the object of type T.
   *
   * Return the pointer to the object of type T. If there
   * isn't a valid object nullptr is returned.
   *
   * @return The pointer to the object of type T.
   */
  T* operator->()
  {
    return optr_.get();
  }

  const T* operator->() const
  {
    return optr_.get();
  }

  T* get()
  {
    return optr_.get();
  }

  const T* get() const
  {
    return optr_.get();
  }
  //@}

  /**
 * Returns the raw object pointer
 *
 * @return The raw object pointer.
 */
  void* ptr()
  {
    return optr_.ptr();
  }

  /**
   * Returns the raw object pointer
   *
   * @return The raw object pointer.
   */
  const void* ptr() const
  {
    return optr_.ptr();
  }

  /**
   * Return the type string of the serializable
   *
   * @return The type string of the serializable.
   */
  const char* type() const
  {
    return optr_.type();
  }

  /**
   * Returns the object id.
   *
   * @return The id of the object.
   */
  unsigned long id() const
  {
    return optr_.id();
  }

  /**
   * Returns the underlying object_ptr
   *
   * @return The underlying object_ptr
   */
  object_ptr<T> optr() const
  {
    return optr_;
  }

  /**
   * Creates a new identifier, represented by the identifier
   * of the underlaying type.
   *
   * @return A new identifier.
   */
  basic_identifier* create_identifier() const override
  {
    return optr_.create_identifier();
  }

  /**
   * Returns true if the underlying object
   * is inserted in an object_store
   *
   * @return True if object is inserted.
   */
  bool is_inserted() const
  {
    return optr_.is_inserted();
  }

  /**
   * Returns true if serializable has a primary key
   *
   * @return true if serializable has a primary key
   */
  bool has_primary_key() const override
  {
    return optr_.has_primary_key();
  }

  /**
   * Gets the primary key of the foreign serializable
   *
   * @return The primary key of the foreign serializable
   */
  std::shared_ptr<basic_identifier> primary_key() const override
  {
    return optr_.primary_key();
  }

private:
  friend class detail::object_deleter;
  friend class detail::object_inserter;

private:
  object_ptr<T> optr_;
};

template < class T >
object_ptr<T>::object_ptr(const has_one<T> &x)
  : object_holder(false, x.optr().proxy_)
{}

template < class T >
object_ptr<T>& object_ptr<T>::operator=(has_one<T> &x)
{
  reset(x.optr().proxy_);
  return *this;
}

}
#endif //OOS_HAS_ONE_HPP
