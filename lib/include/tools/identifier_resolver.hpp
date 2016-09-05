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

#include "cascade_type.hpp"
#include "tools/basic_identifier.hpp"
#include "access.hpp"

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
class identifier_resolver
{
public:
  identifier_resolver() {}
  ~identifier_resolver() {}

  static basic_identifier* resolve(T *o)
  {
    identifier_resolver<T> resolver;
    return resolver.resolve_object(o);
  }

  static basic_identifier* resolve()
  {
    identifier_resolver<T> resolver;
    T obj;
    return resolver.resolve_object(&obj);
  }

  basic_identifier* resolve_object(T *o)
  {
    oos::access::serialize(*this, *o);
    if (!id_) {
      return nullptr;
    }
    return id_;
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
