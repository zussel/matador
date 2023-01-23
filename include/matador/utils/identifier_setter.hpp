#ifndef PRIMARY_KEY_READER_HPP
#define PRIMARY_KEY_READER_HPP

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
  ~identifier_setter() = default;

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
  template < class V >
  void on_primary_key(const char *, V &, long /*size*/ = -1) {}
  void on_primary_key(const char *, T &x, long /*size*/ = -1)
  {
    x = value_;
  }

  template < class V >
  void on_attribute(const char*, V &, long /*size*/ = -1) {}
  void on_attribute(const char*, char *, long /*size*/ = -1) {}
  void on_attribute(const char*, std::string &, long /*size*/ = -1) {}
  void on_belongs_to(const char*, object_holder&, cascade_type) {}
  void on_has_one(const char*, object_holder&, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  T value_;
  bool reading_pk_ = false;
};

/// @endcond

}
#endif /* PRIMARY_KEY_READER_HPP */
