#ifndef CRITERIA_CRITERIA_OPERATORS_HPP
#define CRITERIA_CRITERIA_OPERATORS_HPP

#include "matador/query/criteria/binary_criteria.hpp"
#include "matador/query/criteria/collection_criteria.hpp"
#include "matador/query/intermediates/fetchable_query.hpp"
#include "matador/query/placeholder.hpp"
#include "matador/query/column_value.hpp"

namespace matador::query {
template<class Type>
std::enable_if_t<!std::is_base_of_v<fetchable_query, std::decay_t<Type> >, criteria_ptr>
operator==(const column &col, Type val) {
  return std::make_unique<binary_criteria>(col, binary_operator::Equals, column_value(val));
}

template<class Type>
std::enable_if_t<!std::is_base_of_v<fetchable_query, std::decay_t<Type> >, criteria_ptr>
operator!=(const column &col, Type val) {
  return std::make_unique<binary_criteria>(col, binary_operator::NotEquals, column_value(val));
}

template<class Type>
criteria_ptr operator>(const column &col, Type val) {
  return std::make_unique<binary_criteria>(col, binary_operator::GreaterThan, column_value(val));
}

template<class Type>
criteria_ptr operator>=(const column &col, Type val) {
  return std::make_unique<binary_criteria>(col, binary_operator::GreaterThanOrEqual, column_value(val));
}

template<class Type>
criteria_ptr operator<(const column &col, Type val) {
  return std::make_unique<binary_criteria>(col, binary_operator::LessThan, column_value(val));
}

template<class Type>
criteria_ptr operator<=(const column &col, Type val) {
  return std::make_unique<binary_criteria>(col, binary_operator::LessThanOrEqual, column_value(val));
}

criteria_ptr operator==(const column &col_left, const column &col_right);
criteria_ptr operator!=(const column &col_left, const column &col_right);
criteria_ptr operator>(const column &col_left, const column &col_right);
criteria_ptr operator>=(const column &col_left, const column &col_right);
criteria_ptr operator<(const column &col_left, const column &col_right);
criteria_ptr operator<=(const column &col_left, const column &col_right);

criteria_ptr operator==(const column &col, placeholder p);
criteria_ptr operator!=(const column &col, placeholder p);
criteria_ptr operator>(const column &col, placeholder p);
criteria_ptr operator>=(const column &col, placeholder p);
criteria_ptr operator<(const column &col, placeholder p);
criteria_ptr operator<=(const column &col, placeholder p);

criteria_ptr operator==(const column &col, const identifier &id);
criteria_ptr operator!=(const column &col, const identifier &id);

criteria_ptr operator&&(criteria_ptr left, criteria_ptr right);

criteria_ptr operator||(criteria_ptr left, criteria_ptr right);

criteria_ptr operator!(criteria_ptr clause);

template<class Type>
criteria_ptr in(const column &col, std::initializer_list<Type> args) {
  std::vector<criteria_value> values;
  for (auto &&arg: args) {
    values.emplace_back(column_value{std::move(arg)});
  }
  return std::make_unique<collection_criteria>(col, collection_operator::In, std::move(values));
}

template<>
criteria_ptr in(const column &col, std::initializer_list<placeholder> args);

criteria_ptr in(const column &col, fetchable_query &&q);

template<class Type>
criteria_ptr out(const column &col, std::initializer_list<Type> args) {
  std::vector<criteria_value> values;
  for (auto &&arg: args) {
    values.emplace_back(column_value{std::move(arg)});
  }
  return std::make_unique<collection_criteria>(col, collection_operator::Out, values);
}

template<>
criteria_ptr out(const column &col, std::initializer_list<placeholder> args);
criteria_ptr out(const column &col, fetchable_query &&q);

criteria_ptr between(const column &col, int64_t min, int64_t max);
criteria_ptr between(const column &col, placeholder min, placeholder max);

criteria_ptr like(const column &col, const std::string &pattern);

criteria_ptr is_null(const column &col);
criteria_ptr is_not_null(const column &col);
}
#endif //CRITERIA_CRITERIA_OPERATORS_HPP
