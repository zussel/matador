//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_ONE_HPP
#define OOS_HAS_ONE_HPP

#include "object/object_proxy.hpp"
#include "object/object_ptr.hpp"
#include "object/identifier_resolver.hpp"

namespace oos {

template < class T >
class has_one
{
public:
  typedef T object_type;                    /**< Shortcut for serializable type. */
  typedef has_one<T> self;      /**< Shortcut for self class. */

public:
  enum cascade
  {
    NONE = 0,
    DELETE = 1,
    UPDATE = 2,
    INSERT = 4,
    ALL = DELETE | UPDATE | INSERT
  };

  has_one() {}
  has_one(T *o)
    : proxy_(new object_proxy(o))
  {}
  has_one(object_proxy *proxy)
    : proxy_(proxy)
  {}

  template < bool TYPE >
  has_one& operator=(const object_holder<T, TYPE> &x)
  {
    return *this;
  }

  T* operator->()
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
  object_proxy *proxy_ = nullptr;
  bool is_reference_ = false;
  unsigned long oid_ = 0;

private:
  static std::string classname_;
  static std::unique_ptr<basic_identifier> identifier_;
};

template < class T >
std::string has_one<T>::classname_ = typeid(T).name();

template < class T >
std::unique_ptr<basic_identifier> has_one<T>::identifier_(identifier_resolver<T>::resolve());

}

#endif //OOS_HAS_ONE_HPP
