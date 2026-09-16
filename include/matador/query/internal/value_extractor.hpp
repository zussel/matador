#ifndef QUERY_VALUE_EXTRACTOR_HPP
#define QUERY_VALUE_EXTRACTOR_HPP

#include "matador/query/internal/fk_value_extractor.hpp"

#include "matador/query/access.hpp"
#include "matador/query/value_writer.hpp"
#include "matador/query/default_type_traits.hpp"
#include "matador/query/primary_key_options.hpp"

#include <vector>

namespace matador::query {
class value_extractor final : public value_writer {
private:
  explicit value_extractor(std::vector<utils::database_type> &values);

public:
  template<class Type>
  static std::vector<utils::database_type> extract(const Type &type) {
    std::vector<utils::database_type> values;
    value_extractor gen(values);
    access::process(gen, type);
    return values;
  }

  template<typename BaseType>
  static void on_base(const BaseType &) {
  }

  template<typename ValueType>
  void on_primary_key(const char *, ValueType &x, const primary_key_options &attr) {
    data_type_traits<ValueType>::bind_value(*this, 0, x, attr.size());
  }

  void on_revision(const char *id, const uint64_t &rev);

  template<class Type>
  void on_attribute(const char *, Type &x, const column_options &/*attr*/) {
    data_type_traits<Type>::bind_value(*this, 0, x);
  }

  void on_attribute(const char *id, char *x, const column_options &/*attr*/);
  void on_attribute(const char *id, std::string &x, const column_options &/*attr*/);

  template<class Type, template < class... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &x, const foreign_key_options &/*attr*/) {
    values_.emplace_back(fk_value_extractor_.extract(*x));
  }

  template<class Type, template < class... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &x, const char * /*join_column*/,
                  const foreign_key_options &/*attr*/) {
    values_.emplace_back(fk_value_extractor_.extract(*x));
  }

  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/,
                                  ContainerType &/*c*/,
                                  const char * /*join_column*/,
                                  const char * /*inverse_join_column*/,
                                  const foreign_key_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/,
                                  ContainerType &/*c*/,
                                  const foreign_key_options &/*attr*/) {
  }

public:
  void write_value(size_t pos, const int8_t &x) override;
  void write_value(size_t pos, const int16_t &x) override;
  void write_value(size_t pos, const int32_t &x) override;
  void write_value(size_t pos, const int64_t &x) override;
  void write_value(size_t pos, const uint8_t &x) override;
  void write_value(size_t pos, const uint16_t &x) override;
  void write_value(size_t pos, const uint32_t &x) override;
  void write_value(size_t pos, const uint64_t &x) override;
  void write_value(size_t pos, const bool &x) override;
  void write_value(size_t pos, const float &x) override;
  void write_value(size_t pos, const double &x) override;
  void write_value(size_t pos, const utils::date_type_t &x) override;
  void write_value(size_t pos, const utils::time_type_t &x) override;
  void write_value(size_t pos, const utils::timestamp_type_t &x) override;
  void write_value(size_t pos, const char *x) override;
  void write_value(size_t pos, const char *x, size_t size) override;
  void write_value(size_t pos, const std::string &x) override;
  void write_value(size_t pos, const std::string &x, size_t size) override;
  void write_value(size_t pos, const utils::blob_type_t &x) override;
  void write_value(size_t pos, const column_value &x, size_t size) override;

private:
  detail::fk_value_extractor fk_value_extractor_;
  std::vector<utils::database_type> &values_;
};
}

#endif //QUERY_VALUE_EXTRACTOR_HPP
