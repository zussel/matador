#ifndef QUERY_FK_VALUE_EXTRACTOR_HPP
#define QUERY_FK_VALUE_EXTRACTOR_HPP

#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/primary_key_options.hpp"

#include "matador/utils/types.hpp"

namespace matador::query::detail {
class fk_value_extractor {
public:
  fk_value_extractor() = default;

  template<class Type>
  utils::database_type extract(Type &x) {
    access::process(*this, x);
    return value_;
  }

  template<typename BaseType>
  static void on_base(const BaseType &) {
  }

  template<typename ValueType>
  void on_primary_key(const char *, ValueType &pk, const primary_key_options & /*attr*/) {
    value_ = pk;
  }

  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {
  }

  template<class Type>
  static void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) {
  }

  static void on_attribute(const char * /*id*/, char * /*x*/, const column_options &/*attr*/) {
  }

  template<class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_options &/*attr*/) {
  }

  template<class Pointer>
  static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/,
                         const foreign_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const char * /*join_column*/,
                                  const char * /*inverse_join_column*/, const foreign_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const foreign_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many(const char *, ContainerType &, const char * /*join_column*/,
                          const foreign_options &/*attr*/) {
  }

private:
  utils::database_type value_{};
};
}

#endif //QUERY_FK_VALUE_EXTRACTOR_HPP
