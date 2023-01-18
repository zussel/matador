#ifndef PRIMARY_KEY_SERIALIZER_HPP
#define PRIMARY_KEY_SERIALIZER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/id_pk.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/basic_identifier.hpp"
#include "matador/utils/access.hpp"

#include <stdexcept>

namespace matador {

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

  static id_pk resolve(T *o)
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

  id_pk resolve_object(T *o)
  {
    matador::access::serialize(*this, *o);
    return id_;
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class V >
  void on_attribute(const char*, V&, long /*size*/ = -1) {}

  void on_belongs_to(const char*, identifiable_holder&, cascade_type) {}
  void on_has_one(const char*, identifiable_holder&, cascade_type) {}

  void on_has_many(const char *, abstract_has_many&, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many&, cascade_type) {}

  template < class V >
  void on_primary_key(const char *, V &x, long /*size*/ = -1)
  {
    id_ = x;
  }

private:
  bool clone_ = false;
  id_pk id_;
};

/// @endcond

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
