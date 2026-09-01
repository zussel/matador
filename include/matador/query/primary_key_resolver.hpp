#ifndef PRIMARY_KEY_RESOLVER_HPP
#define PRIMARY_KEY_RESOLVER_HPP

#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/primary_key_options.hpp"

#include "matador/utils/default_type_traits.hpp"

#include <cstdint>
#include <string>

namespace matador::query {
class foreign_attributes;
}

namespace matador::query {

struct primary_key_info {
  std::string pk_column_name;
  utils::basic_type type;
  identifier pk;
};

class primary_key_resolver final {
public:
  template <typename Type>
  primary_key_info resolve() {
    Type obj;

    return resolve(obj);
  }

  template <typename Type>
  primary_key_info resolve(const Type& obj) {
    field::process(*this, obj);

    return primary_key_info_;
  }

  template <typename Type>
  static primary_key_info resolve_object(const Type& obj) {
    primary_key_resolver resolver;
    return resolver.resolve(obj);
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template < class Type >
  void on_primary_key(const char *id, Type &pk, const primary_key_options& attr) {
    primary_key_info_.pk_column_name = id;
    primary_key_info_.type = utils::data_type_traits<Type>::type(attr.size());
    primary_key_info_.pk = pk;
  }

  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template<typename Type>
  static void on_attribute(const char * /*id*/, Type &/*val*/, const column_options &/*attr*/) {}
  template<class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer &/*val*/, const foreign_options &/*attr*/) {}
  template<class Pointer>
  static void on_has_one(const char * /*id*/, Pointer &/*val*/, const char * /*join_column*/, const foreign_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &/*col*/, const char *, const foreign_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType &/*col*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType &/*col*/, const foreign_options &/*attr*/) {}

private:
  primary_key_info primary_key_info_{};
};

}
#endif //PRIMARY_KEY_RESOLVER_HPP
