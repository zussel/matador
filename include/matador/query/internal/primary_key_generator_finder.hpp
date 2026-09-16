#ifndef MATADOR_PRIMARY_KEY_GENERATOR_FINDER_HPP
#define MATADOR_PRIMARY_KEY_GENERATOR_FINDER_HPP

#include "matador/query/access.hpp"
#include "matador/query/primary_key_generator_type.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/table_info.hpp"

namespace matador::query {
class column_options;
class foreign_ley_options;
}
namespace matador::query::internal {
class primary_key_generator_finder final {
public:
  template< typename Type >
  generator_type find(const table_info<Type>& info) {
    generator_type_ = generator_type::Manual;

    access::process(*this, info.prototype());

    return generator_type_;
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<class V>
  void on_primary_key(const char * /*id*/, V &/*pk*/, const primary_key_options &attr) {
    generator_type_ = attr.generator();
  }
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}
  template<typename T>
  static void on_attribute(const char * /*id*/, T &, const column_options &/*attr*/) {}
  template<class P>
  static void on_belongs_to(const char * /*id*/, P &, const foreign_ley_options & ) {}
  template<class P>
  static void on_has_one(const char * /*id*/, P &, const char * /*join_column*/, const foreign_ley_options & ) {}
  template<class C>
  static void on_has_many(const char * /*id*/, C &, const char * /*join_column*/, const foreign_ley_options & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_ley_options & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const foreign_ley_options & ) {}

private:
  generator_type generator_type_{};
};
}
#endif // MATADOR_PRIMARY_KEY_GENERATOR_FINDER_HPP
