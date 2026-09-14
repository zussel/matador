#ifndef MATADOR_OBJECT_RESULT_BINDER_HPP
#define MATADOR_OBJECT_RESULT_BINDER_HPP

#include "matador/query/default_type_traits.hpp"
#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/foreign_key_options.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/value_reader.hpp"

namespace matador::query {

namespace detail {
class fk_result_binder {
public:
  template <class Type>
  void bind(Type &obj, const char *id, const size_t column_index, value_reader &binder) {
    binder_ = &binder;
    index_ = column_index;
    id_ = id;
    access::process(*this, obj);
    id_ = nullptr;
    binder_ = nullptr;
  }

  template <typename BaseType> static void on_base(const BaseType &) {}
  template <typename ValueType>
  void on_primary_key(const char *id, ValueType &value, const primary_key_options &attr);
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}

  template <class Type>
  static void on_attribute(const char * /*id*/, Type & /*x*/, const column_options & /*attr*/) {}
  template <class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer & /*x*/, const foreign_key_options & /*attr*/) {}
  template <class Pointer>
  static void on_has_one(const char * /*id*/, Pointer & /*x*/, const char * /*join_column*/, const foreign_key_options & /*attr*/) {}

  template <class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType & /*c*/, const char * /*join_column*/, const foreign_key_options & /*attr*/) {}
  template <class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/,
                                  const char * /*join_column*/,
                                  const char * /*inverse_join_column*/,
                                  const foreign_key_options & /*attr*/) {}
  template <class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/, const foreign_key_options & /*attr*/) {}

private:
  value_reader *binder_{};
  size_t index_{0};
  const char *id_{};
};

} // namespace detail

class object_result_binder {
public:
  template <class Type> void bind(Type &obj, value_reader &binder) {
    binder_ = &binder;
    access::process(*this, obj);
    binder_ = nullptr;
  }

  void reset();

  template <typename BaseType> static void on_base(const BaseType &) {}
  template <class Type>
  void on_primary_key(const char *id, Type &val, const primary_key_options &attr) {
    data_type_traits<Type>::read_value(*binder_, id, index_++, val, attr.size());
  }
  void on_revision(const char *id, uint64_t & /*rev*/);

  template <typename Type>
  void on_attribute(const char *id, Type &val, const column_options & /*attr*/) {
    data_type_traits<Type>::read_value(*binder_, id, index_++, val);
  }
  void on_attribute(const char *id, char *value, const column_options &attr);
  void on_attribute(const char *id, std::string &value, const column_options &attr);
  void on_attribute(const char *id, column_value &val, const column_options &attr);

  template <class Type, template <class...> class Pointer>
  void on_belongs_to(const char *id, Pointer<Type> &x, const foreign_key_options & /*attr*/) {
    fk_result_binder_.bind(*x, id, index_++, *binder_);
  }
  template <class Type, template <class...> class Pointer>
  void on_has_one(const char *id, Pointer<Type> &x, const char * /*join_column*/,
                  const foreign_key_options & /*attr*/) {
    fk_result_binder_.bind(*x, id, index_++, *binder_);
  }
  template <class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType & /*c*/, const char * /*join_column*/,
                          const foreign_key_options & /*attr*/) {}
  template <class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/,
                                  const char * /*join_column*/,
                                  const char * /*inverse_join_column*/,
                                  const foreign_key_options & /*attr*/) {}
  template <class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/,
                                  const foreign_key_options & /*attr*/) {}

private:
  value_reader *binder_{};
  size_t index_{0};
  detail::fk_result_binder fk_result_binder_;
};

namespace detail {

template <typename ValueType>
void fk_result_binder::on_primary_key(const char * /*id*/, ValueType &value,
                                      const primary_key_options &attr) {
  data_type_traits<ValueType>::read_value(*binder_, id_, index_++, value, attr.size());
}

} // namespace detail

} // namespace matador::query

#endif // MATADOR_OBJECT_RESULT_BINDER_HPP
