#ifndef MATADOR_PK_READER_HPP
#define MATADOR_PK_READER_HPP

#include "matador/query/interface/query_result_reader.hpp"

namespace matador::query {
class primary_key_reader {
public:
  explicit primary_key_reader(query_result_reader &reader)
  : reader_(reader) {}

  template<class Type>
  void read(Type &obj, const size_t column_index) {
    column_index_ = column_index;
    access::process(*this, obj);
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, const primary_key_options& attr);
  void on_revision(const char * /*id*/, uint64_t &/*rev*/) {
    ++column_index_;
  }

  template<class Type>
  void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) {
    ++column_index_;
  }

  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_key_options &/*attr*/) {
    ++column_index_;
  }
  template<class Pointer>
  static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char * /*join_column*/, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType &c, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType &c, const foreign_key_options &/*attr*/) {}

private:
  size_t column_index_{};
  query_result_reader &reader_;
};

template<typename ValueType>
void primary_key_reader::on_primary_key(const char *id, ValueType &value, const primary_key_options& attr) {
  data_type_traits<ValueType>::read_value(reader_, id, column_index_++, value, attr.size());
}
}
#endif  // MATADOR_PK_READER_HPP
