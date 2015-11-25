//
// Created by sascha on 4/10/15.
//

#ifndef PRIMARY_KEY_SERIALIZER_HPP
#define PRIMARY_KEY_SERIALIZER_HPP

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

#include "object/serializer.hpp"
#include "object/serializable.hpp"

#include <stdexcept>

namespace oos {

class serializable;
class object_base_ptr;
class basic_identifier;
class prototype_node;

/// @cond OOS_DEV

/**
 * @class identifier_resolver
 * @brief Returns the primary key of an object
 *
 * Returns the primary key class of a serializable
 * object. If object doesn't have a primary key
 * nullptr is returned
 */
class OOS_API identifier_resolver : public generic_deserializer<identifier_resolver>
{
public:
  identifier_resolver();
  virtual ~identifier_resolver();

  template < class T >
  static basic_identifier* resolve(typename std::enable_if<!std::is_same<T, oos::serializable>::value>::type* = 0)
  {
    T obj;

    return resolve(&obj);
  }

  template < class T >
  static basic_identifier* resolve(typename std::enable_if<std::is_same<T, oos::serializable>::value>::type* = 0)
  {
    return nullptr;
  }

  static basic_identifier* resolve(serializable *obj)
  {
    identifier_resolver resolver;
    obj->deserialize(resolver);
    if (!resolver.id_) {
      return nullptr;
    }
    return resolver.id_;
  }

  template < class T >
  void read_value(const char*, T&) {}

  void read_value(const char*, char*, size_t) {}
  void read_value(const char *id, basic_identifier &x);

private:
  basic_identifier *id_ = nullptr;
};

/// @endcond

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
