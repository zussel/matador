#ifndef PRIMARY_KEY_READER_HPP
#define PRIMARY_KEY_READER_HPP

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/access.hpp"
#include "matador/utils/foreign_attributes.hpp"

namespace matador {

/// @cond MATADOR_DEV

class object_holder;
class abstract_container;

template < class T, class Enabled = void >
class identifier_setter;

template < class T >
class identifier_setter<T, typename std::enable_if<std::is_integral<T>::value>::type>
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
    matador::access::process(setter, *obj);
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::process(*this, x);
  }

  template < class V >
  void on_primary_key(const char *, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    x = static_cast<V>(value_);
  }
  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<!std::is_integral<V>::value || std::is_same<bool, V>::value>::type* = 0) { }
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}


  template < class V >
  void on_attribute(const char*, V &, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_attribute(const char*, char *, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_attribute(const char*, std::string &, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}

  void on_belongs_to(const char*, identifiable_holder&, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_one(const char*, identifiable_holder&, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_many(const char *, abstract_container&, const char * /*join_column*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_many(const char *, abstract_container&, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_many_to_many(const char *, abstract_container&, const char * /*join_column*/, const char * /*inverse_join_column*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_many_to_many(const char *, abstract_container&, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}

private:
  T value_;
};

template < class T >
class identifier_setter<T, typename std::enable_if<std::is_same<T, std::string>::value>::type>
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
    matador::access::process(setter, *obj);
  }

  template < class V >
  void serialize(V &x)
  {
    matador::access::process(*this, x);
  }

  template < class V >
  void on_primary_key(const char *, V &, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_primary_key(const char *, T &x, size_t /*size*/)
  {
    x = value_;
  }

  template < class V >
  void on_attribute(const char*, V &, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_attribute(const char*, char *, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_attribute(const char*, std::string &, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_belongs_to(const char*, object_holder&, cascade_type) {}
  void on_has_one(const char*, object_holder&, cascade_type) {}
  void on_has_many(const char *, abstract_container&, const char * /*join_column*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_many_to_many(const char *, abstract_container&, const char * /*join_column*/, const char * /*inverse_join_column*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  void on_has_many_to_many(const char *, abstract_container&, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}

private:
  T value_;
};

/// @endcond

}
#endif /* PRIMARY_KEY_READER_HPP */
