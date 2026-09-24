#ifndef QUERY_OBJECT_PARAMETER_BINDER_HPP
#define QUERY_OBJECT_PARAMETER_BINDER_HPP

#include "matador/query/internal/primary_key_binder.hpp"

namespace matador::query {

class object_binder {
public:
  template<class Type>
  void bind(Type &obj, value_writer &binder) {
    binder_ = &binder;
    access::process(*this, obj);
    binder_ = nullptr;
  }

  void reset(size_t start_index);
  [[nodiscard]] size_t current_index() const;

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template < class Type >
  void on_primary_key(const char * /*id*/, Type &val, const primary_key_options& attr) {
    if (attr.generator() == generator_type::Identity) {
      return;
    }
    data_type_traits<Type>::bind_value(*binder_, index_++, val, attr.size());
  }
  void on_revision(const char *id, uint64_t &/*rev*/);

  template<typename Type>
  void on_attribute(const char * /*id*/, Type &val, const column_options &/*attr*/) {
    data_type_traits<Type>::bind_value(*binder_, index_++, val);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &x, const foreign_key_options &/*attr*/) {
    pk_binder_.bind(*x, index_++, *binder_);
  }
  template<class Type, template < class ... > class Pointer>
  static void on_has_one(const char * /*id*/,
                         Pointer<Type> &/*x*/,
                         const char * /*join_column*/,
                         const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many(const char * /*id*/,
                          ContainerType &/*c*/,
                          const char * /*join_column*/,
                          const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/,
                                  ContainerType &/*c*/,
                                  const char * /*join_column*/,
                                  const char * /*inverse_join_column*/,
                                  const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/,
                                  ContainerType &/*c*/,
                                  const foreign_key_options &/*attr*/) {}

private:
  value_writer *binder_{};
  size_t index_{0};
  primary_key_binder pk_binder_;
};
}
#endif //QUERY_OBJECT_PARAMETER_BINDER_HPP
