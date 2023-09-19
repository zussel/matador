#ifndef PRIMARY_KEY_SERIALIZER_HPP
#define PRIMARY_KEY_SERIALIZER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/access.hpp"

#include <stdexcept>

namespace matador {

class identifiable_holder;
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

  static identifier resolve(T *o)
  {
    identifier_resolver<T> resolver;
    return resolver.resolve_object(o);
  }

  static identifier create()
  {
    identifier_resolver<T> resolver;
    T obj;
    return resolver.resolve_object(&obj);
  }

  identifier resolve_object(T *o)
  {
    matador::access::process(*this, *o);
    return id_;
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::process(*this, x);
  }

  template < class V >
  void on_primary_key(const char *, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    id_ = x;
  }
  void on_primary_key(const char *, std::string &pk, size_t /*size*/)
  {
    id_ =  pk;
  }
  void on_revision(const char *, unsigned long long &/*rev*/) {}

  template < class V >
  void on_attribute(const char*, V&, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char*, identifiable_holder&, cascade_type) {}

  void on_has_one(const char*, identifiable_holder&, cascade_type) {}
  void on_has_many(const char *, abstract_has_many&, const char *, const char *, cascade_type) {}

  void on_has_many(const char *, abstract_has_many&, cascade_type) {}

private:
  identifier id_;
};

/// @endcond

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
