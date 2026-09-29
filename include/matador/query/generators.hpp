#ifndef MATADOR_GENERATORS_HPP
#define MATADOR_GENERATORS_HPP

#include "matador/query/access.hpp"
#include "matador/query/placeholder.hpp"

#include <vector>

namespace matador::query {
class primary_key_options;
class foreign_key_options;
class column_options;

class placeholder_generator final {
public:
  template< class Type >
  std::vector<placeholder> generate() {
    Type obj;
    return generate(obj);
  }

  template< class Type >
  std::vector<placeholder> generate(const Type &obj) {
    result_.clear();
    access::process(*this, obj);

    return result_;
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template < class V >
  void on_primary_key(const char * /*id*/, V &/*x*/, const primary_key_options& /*attr*/) {
    result_.emplace_back(_);
  }
  void on_revision(const char *id, uint64_t &/*rev*/);

  template<typename Type>
  void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) {
    result_.emplace_back(_);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &/*x*/, const foreign_key_options &/*attr*/) {
    result_.emplace_back(_);
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &/*x*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {
    result_.emplace_back(_);
  }
  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const char *, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const foreign_key_options &/*attr*/) {}

private:
  std::vector<placeholder> result_;
};

template<typename Type>
std::vector<placeholder> placeholders() {
  placeholder_generator generator;
  return generator.generate<Type>();
}

template<typename Type>
std::vector<placeholder> placeholders(const Type &obj) {
  placeholder_generator generator;
  return generator.generate(obj);
}

std::vector<placeholder> placeholders(size_t num);

}
#endif // MATADOR_GENERATORS_HPP
