//
// Created by sascha on 6/22/15.
//

#ifndef PRIMARY_KEY_READER_HPP
#define PRIMARY_KEY_READER_HPP

#include "tools/identifier.hpp"

#include "object/cascade_type.hpp"
#include "object/access.hpp"

namespace oos {

/// @cond OOS_DEV

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
  void serialize(V &x)
  {
    oos::access::serialize(*this, x);
  }
  void serialize(const char*, T &x);

  template < class V >
  void serialize(const char *id, identifier<V> &x)
  {
    reading_pk_ = true;
    // Todo: check correctness
    V val;
    serialize(id, val);
    x.value(val);
    reading_pk_ = false;
  }

  template < class V >
  void serialize(const char*, V &) {}
  void serialize(const char*, char *, size_t) {}
  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) {}

  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

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
