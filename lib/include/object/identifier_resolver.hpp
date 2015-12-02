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

#include "object/basic_identifier.hpp"
#include "object/access.hpp"

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
template < class T >
class OOS_API identifier_resolver
{
public:
  identifier_resolver() {}
  ~identifier_resolver() {}

  static basic_identifier* resolve()
  {
    identifier_resolver<T> resolver;
    T obj;
    oos::access::deserialize(resolver, obj);
    if (!resolver.id_) {
      return nullptr;
    }
    return resolver.id_;
  }

  template < class V >
  void deserialize(V &x)
  {
    oos::access::deserialize(*this, x);
  }

  template < class V >
  void deserialize(const char*, V&) {}

  void deserialize(const char*, char*, size_t) {}
  void deserialize(const char *id, basic_identifier &x)
  {
    id_ = x.share();
  }

private:
  basic_identifier *id_ = nullptr;
};

/// @endcond

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
