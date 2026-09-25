#ifndef MATADOR_PRIMARY_KEY_ACCESSOR_HPP
#define MATADOR_PRIMARY_KEY_ACCESSOR_HPP

#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/primary_key_options.hpp"

#include <cstdint>
#include <string>
#include <utility>

namespace matador::query {
class foreign_attributes;
namespace detail {
class primary_key_setter {
public:
  template <typename ObjectType>
  void set(ObjectType &obj, const identifier& pk) {
    pk_ = pk;
    access::process(*this, obj);
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<typename PrimaryKeyType>
  void on_primary_key(const char * /*id*/, PrimaryKeyType &pk, const primary_key_attribute & = DefaultPkAttributes) {
    const auto value = pk_.convert<PrimaryKeyType>();
    if (!value) {
      // Todo: throw error
    }

    pk = value.value();
  }
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}
  template<typename T>
  static void on_attribute(const char * /*id*/, T &, const field_attributes & = NullAttributes) {}
  template<class P>
  static void on_belongs_to(const char * /*id*/, P &, const foreign_attributes & ) {}
  template<class P>
  static void on_has_one(const char * /*id*/, P &, const char * /*join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many(const char * /*id*/, C &, const char * /*join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const foreign_attributes & ) {}

private:
  identifier pk_{};
};

struct pk_unset_checker {
  bool unset{true};

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<class PrimaryKeyType>
  void on_primary_key(const char * /*id*/, PrimaryKeyType &pk, const primary_key_attribute & = DefaultPkAttributes) {
    unset = !identifier_type_traits<PrimaryKeyType>::is_valid(pk);
  }
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}
  template<typename T>
  static void on_attribute(const char * /*id*/, T &, const field_attributes & = NullAttributes) {}
  template<class P>
  static void on_belongs_to(const char * /*id*/, P &, const foreign_attributes & ) {}
  template<class P>
  static void on_has_one(const char * /*id*/, P &, const char * /*join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many(const char * /*id*/, C &, const char * /*join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const foreign_attributes & ) {}
};

struct primary_key_getter {
  std::uint64_t value{0};

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<class PrimaryKeyType>
  void on_primary_key(const char * /*id*/, PrimaryKeyType &pk, const primary_key_attribute & = DefaultPkAttributes) {
    pk_ = pk;
  }
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}
  template<typename T>
  static void on_attribute(const char * /*id*/, T &, const field_attributes & = NullAttributes) {}
  template<class P>
  static void on_belongs_to(const char * /*id*/, P &, const foreign_attributes & ) {}
  template<class P>
  static void on_has_one(const char * /*id*/, P &, const char * /*join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many(const char * /*id*/, C &, const char * /*join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_attributes & ) {}
  template<class C>
  static void on_has_many_to_many(const char * /*id*/, C &, const foreign_attributes & ) {}

  identifier pk_;
};

}

class primary_key_accessor {
public:
  template<class Type>
  identifier get(const Type &obj) {
    access::process(pk_getter_, obj);
    return pk_getter_.pk_;
  }

  template<class Type>
  void set(Type &obj, const identifier &pk) {
    pk_setter_.set(obj, pk);
  }

  template<class Type>
  bool is_set(const Type &obj) {
    access::process(pk_unset_checker_, obj);
    return !pk_unset_checker_.unset;
  }

private:
  detail::primary_key_setter pk_setter_;
  detail::pk_unset_checker pk_unset_checker_;
  detail::primary_key_getter pk_getter_;

};
}

#endif //MATADOR_PRIMARY_KEY_ACCESSOR_HPP