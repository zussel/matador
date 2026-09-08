#include "matador/query/criteria/criteria_operators.hpp"

#include "matador/query/criteria/between_criteria.hpp"
#include "matador/query/criteria/check_null_criteria.hpp"
#include "matador/query/criteria/like_criteria.hpp"
#include "matador/query/criteria/logical_criteria.hpp"
#include "matador/query/criteria/not_criteria.hpp"

#include "matador/query/identifier_serializer.hpp"

namespace {
class identifier_to_value_converter final : public matador::query::identifier_serializer {
public:
  [[nodiscard]] matador::query::column_value convert(const matador::query::identifier &id) {
    id.serialize(*this);

    return value_;
  }

  void serialize(int8_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(int16_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(int32_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(int64_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(uint8_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(uint16_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(uint32_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(uint64_t &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(std::string &x, const matador::query::column_options &) override { value_ = x; }
  void serialize(matador::utils::null_type_t &/*x*/, const matador::query::column_options &) override { value_.type(matador::utils::basic_type::Null); }

private:
  matador::query::column_value value_;
};
}

namespace matador::query {
criteria_ptr operator==(const column &col, const identifier &id) {
  identifier_to_value_converter conv;

  return std::make_unique<binary_criteria>(col, binary_operator::Equals, conv.convert(id));
}

criteria_ptr operator!=(const column &col, const identifier &id) {
  identifier_to_value_converter conv;

  return std::make_unique<binary_criteria>(col, binary_operator::NotEquals, conv.convert(id));
}

criteria_ptr operator==(const column &col, placeholder p) {
  return std::make_unique<binary_criteria>(col, binary_operator::Equals, p);
}

criteria_ptr operator!=(const column &col, placeholder p) {
  return std::make_unique<binary_criteria>(col, binary_operator::NotEquals, p);
}

criteria_ptr operator>(const column &col, placeholder p) {
  return std::make_unique<binary_criteria>(col, binary_operator::GreaterThan, p);
}

criteria_ptr operator>=(const column &col, placeholder p) {
  return std::make_unique<binary_criteria>(col, binary_operator::GreaterThanOrEqual, p);
}

criteria_ptr operator<(const column &col, placeholder p) {
  return std::make_unique<binary_criteria>(col, binary_operator::LessThan, p);
}

criteria_ptr operator<=(const column &col, placeholder p) {
  return std::make_unique<binary_criteria>(col, binary_operator::LessThanOrEqual, p);
}

criteria_ptr operator==( const column& col_left, const column& col_right ) {
    return std::make_unique<binary_column_criteria>(col_left, binary_operator::Equals, col_right);
}

criteria_ptr operator!=( const column& col_left, const column& col_right ) {
    return std::make_unique<binary_column_criteria>(col_left, binary_operator::NotEquals, col_right);
}

criteria_ptr operator>( const column& col_left, const column& col_right ) {
    return std::make_unique<binary_column_criteria>(col_left, binary_operator::GreaterThan, col_right);
}

criteria_ptr operator>=( const column& col_left, const column& col_right ) {
    return std::make_unique<binary_column_criteria>(col_left, binary_operator::GreaterThanOrEqual, col_right);
}

criteria_ptr operator<( const column& col_left, const column& col_right ) {
    return std::make_unique<binary_column_criteria>(col_left, binary_operator::LessThan, col_right);
}

criteria_ptr operator<=( const column& col_left, const column& col_right ) {
    return std::make_unique<binary_column_criteria>(col_left, binary_operator::LessThanOrEqual, col_right);
}

criteria_ptr operator&&(criteria_ptr left, criteria_ptr right) {
  return std::make_unique<logical_criteria>(std::move(left), logical_operator::AND, std::move(right));
}

criteria_ptr operator||(criteria_ptr left, criteria_ptr right) {
  return std::make_unique<logical_criteria>(std::move(left), logical_operator::OR, std::move(right));
}

criteria_ptr operator!(criteria_ptr clause) {
  return std::make_unique<not_criteria>(std::move(clause));
}

template <>
criteria_ptr in(const column &col, const std::initializer_list<placeholder> args) {
  std::vector<criteria_value> values;
  for ( auto &&arg : args ) {
    values.emplace_back(arg);
  }
  return std::make_unique<collection_criteria>(col, collection_operator::In, std::move(values));
}

criteria_ptr in(const column &col, query_context &&q) {
  return std::make_unique<collection_query_criteria>(col, collection_operator::In, std::move(q));
}

template <>
criteria_ptr out(const column &col, const std::initializer_list<placeholder> args) {
  std::vector<criteria_value> values;
  for ( auto &&arg : args ) {
    values.emplace_back(arg);
  }
  return std::make_unique<collection_criteria>(col, collection_operator::Out, values);
}

criteria_ptr out(const column &col, query_context &&q) {
  return std::make_unique<collection_query_criteria>(col, collection_operator::In, std::move(q));
}

criteria_ptr between(const column &col, const int64_t min, const int64_t max) {
  return std::make_unique<between_criteria>(col, min, max);
}

criteria_ptr between(const column &col, placeholder min, placeholder max) {
  return std::make_unique<between_criteria>(col, min, max);
}

criteria_ptr like(const column &col, const std::string &pattern) {
  return std::make_unique<like_criteria>(col, pattern);
}

criteria_ptr is_null(const column &col) {
  return std::make_unique<check_null_criteria>(col, check_null_operator::IsNull);
}
criteria_ptr is_not_null(const column &col) {
  return std::make_unique<check_null_criteria>(col, check_null_operator::IsNotNull);
}
}  // namespace matador::query
