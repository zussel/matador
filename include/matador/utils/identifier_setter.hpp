//
// Created by sascha on 6/22/15.
//

#ifndef PRIMARY_KEY_READER_HPP
#define PRIMARY_KEY_READER_HPP

#include "matador/utils/identifier.hpp"

#include "cascade_type.hpp"
#include "access.hpp"

namespace matador {

/// @cond MATADOR_DEV

class object_holder;
class abstract_has_many;

template < class T >
class identifier_setter
{

public:
  explicit identifier_setter(const T &value)
    : value_(value)
  {
    value_ = value;
  }
  ~identifier_setter() { }

public:

  template < class V >
  static void assign(T value, V *obj)
  {
    identifier_setter<T> setter(value);
    matador::access::serialize(setter, *obj);
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::serialize(*this, x);
  }
  void serialize(const char*, T &x);

  template < class V >
  void serialize(const char *id, identifier<V> &x)
  {
    reading_pk_ = true;
    V val;
    serialize(id, val);
    x.value(val);
    reading_pk_ = false;
  }

  template < class V >
  void serialize(const char*, V &) {}
  void serialize(const char*, char *, size_t) {}
//  template < class HAS_ONE >
  void serialize(const char*, object_holder&, cascade_type) {}

//  template < class HAS_MANY >
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void serialize(const char *, abstract_has_many &, cascade_type) {}

private:
  T value_;
  bool reading_pk_ = false;
};

template < class T >
void identifier_setter<T>::serialize(const char*, T &x) {
  if (reading_pk_) {
    x = value_;
  }
}

/// @endcond

}
#endif /* PRIMARY_KEY_READER_HPP */
