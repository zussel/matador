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

#include "object/cascade_type.hpp"
#include "tools/basic_identifier.hpp"
#include "object/access.hpp"

#include <stdexcept>

namespace oos {

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

  static basic_identifier* resolve(T *o)
  {
    identifier_resolver<T> resolver;
    oos::access::serialize(resolver, *o);
    if (!resolver.id_) {
      return nullptr;
    }
    return resolver.id_;
  }

  static basic_identifier* resolve()
  {
    identifier_resolver<T> resolver;
    T obj;
    oos::access::serialize(resolver, obj);
    if (!resolver.id_) {
      return nullptr;
    }
    return resolver.id_;
  }

  template < class V >
  void serialize(V &x)
  {
    oos::access::serialize(*this, x);
  }

  template < class V >
  void serialize(const char*, V&) {}

  void serialize(const char*, char*, size_t) {}

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) {}

  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

  template < class V >
  void serialize(const char *, identifier<V> &x)
  {
    id_ = x.share();
  }

private:
  basic_identifier *id_ = nullptr;
};

/// @endcond

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
