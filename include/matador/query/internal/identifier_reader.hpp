#ifndef MATADOR_IDENTIFIER_READER_HPP
#define MATADOR_IDENTIFIER_READER_HPP

#include "matador/query/access.hpp"
#include "matador/query/type_traits.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/primary_key_options.hpp"

namespace matador::query {
class query_result_reader;

class identifier_reader final {
public:
  explicit identifier_reader(query_result_reader &reader)
  : reader_(reader) {}

  template<class Type>
  identifier read(const Type &obj, const size_t column_index) {
    identifier_.clear();
    column_index_ = column_index;
    access::process(*this, obj);
    return identifier_;
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, const primary_key_options& attr) {
    data_type_traits<ValueType>::read_value(reader_, id, column_index_, value, attr.size());
    identifier_ = value;
  }

  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template < class Type >
  static void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) {}
  template < class Pointer >
  static void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_key_options &/*attr*/) {}
  template < class Pointer >
  static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/, const foreign_key_options &/*attr*/) {}

private:
  query_result_reader &reader_;
  size_t column_index_{};
  identifier identifier_;
};
}
#endif //MATADOR_IDENTIFIER_READER_HPP