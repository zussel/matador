#ifndef PRIMARY_KEY_SERIALIZER_HPP
#define PRIMARY_KEY_SERIALIZER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/access.hpp"

#include <stdexcept>

namespace matador {

class basic_identifier;
class prototype_node;
class object_holder;
class abstract_has_many;

/// @cond MATADOR_DEV

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
  identifier_resolver() = default;
  ~identifier_resolver() = default;

  static basic_identifier* resolve(T *o)
  {
    identifier_resolver<T> resolver;
    return resolver.resolve_object(o);
  }

  static basic_identifier* create()
  {
    identifier_resolver<T> resolver;
    T obj;
    return resolver.resolve_object(&obj, true);
  }

  basic_identifier* resolve_object(T *o, bool clone = false)
  {
    clone_ = clone;
    matador::access::serialize(*this, *o);
    clone_ = false;
    if (!id_) {
      return nullptr;
    }
    return id_;
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class V >
  void serialize(const char*, V&) {}

  void serialize(const char*, char*, size_t) {}
  void serialize(const char*, std::string &, size_t) {}

//  template < class HAS_ONE >
  void serialize(const char*, object_holder&, cascade_type) {}

//  template < class HAS_MANY >
  void serialize(const char *, abstract_has_many&, const char *, const char *, cascade_type) {}
  void serialize(const char *, abstract_has_many&, cascade_type) {}

  template < class V >
  void serialize(const char *, identifier<V> &x)
  {
    if (clone_) {
      id_ = x.clone();
    } else {
//      id_ = x.clone();
      id_ = &x;
    }
  }

private:
  bool clone_ = false;
  basic_identifier *id_ = nullptr;
};

/// @endcond

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
