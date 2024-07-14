#ifndef OOS_IDENTIFIER_COLUMN_RESOLVER_HPP
#define OOS_IDENTIFIER_COLUMN_RESOLVER_HPP

#include "matador/orm/export.hpp"

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"
#include "matador/utils/access.hpp"

#include "matador/sql/column.hpp"

#include <cstddef>

namespace matador {

class object_holder;
class abstract_container;

namespace detail {

/// @cond MATADOR_DEV

class OOS_ORM_API identifier_column_resolver
{
public:
  identifier_column_resolver() : col_("") {}

  template < class T >
  static column resolve();

  template < class T >
  column resolve(T *obj);

  template<class T>
  void serialize(T &x)
  {
    matador::access::process(*this, x);
  }

  template < class V >
  void on_primary_key(const char *, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  void on_primary_key(const char *id, std::string &pk, size_t size);
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class T>
  void on_attribute(const char *, T &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char*, identifiable_holder&, const foreign_attributes &/*attr*/ = default_foreign_attributes) {}
  void on_has_one(const char*, identifiable_holder&, const foreign_attributes &/*attr*/ = default_foreign_attributes) {}
  void on_has_many(const char *, abstract_container&, const char * /*join_column*/, const foreign_attributes &/*attr*/ = default_foreign_attributes) {}
  void on_has_many(const char *, abstract_container&, const foreign_attributes &/*attr*/ = default_foreign_attributes) {}
  void on_has_many_to_many(const char *, abstract_container&, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_attributes &/*attr*/ = default_foreign_attributes) {}
  void on_has_many_to_many(const char *, abstract_container&, const foreign_attributes &/*attr*/ = default_foreign_attributes) {}

private:
  column col_;
};

template<class T>
column identifier_column_resolver::resolve()
{
  identifier_column_resolver resolver;
  T obj;
  return resolver.resolve(&obj);
}

template<class T>
column identifier_column_resolver::resolve(T *obj)
{
  matador::access::process(*this, *obj);
  return col_;
}

template<class V>
void identifier_column_resolver::on_primary_key(const char *id, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type*)
{
  col_.name = id;
}

/// @endcond

}
}

#endif //OOS_IDENTIFIER_COLUMN_RESOLVER_HPP
