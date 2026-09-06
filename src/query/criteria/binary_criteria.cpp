#include <utility>

#include "matador/query/criteria/binary_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"

namespace matador::query {
binary_criteria::binary_criteria(const column& col, const binary_operator operand, criteria_value value)
: abstract_column_criteria(col)
, operator_(operand)
, value_(std::move(value))
{}

void binary_criteria::accept( criteria_visitor& visitor ) const {
  visitor.visit(*this);
}

binary_operator binary_criteria::operand() const {
  return operator_;
}

const criteria_value& binary_criteria::value() const {
  return value_;
}

binary_column_criteria::binary_column_criteria(column  left_column, const binary_operator operand, column  right_column)
: left_column_(std::move(left_column))
, operator_(operand)
, right_column_(std::move(right_column)){}

void binary_column_criteria::accept(criteria_visitor& visitor) const {
  visitor.visit(*this);
}

const column& binary_column_criteria::left_column() const {
  return left_column_;
}

binary_operator binary_column_criteria::operand() const {
  return operator_;
}

const column& binary_column_criteria::right_column() const {
  return right_column_;
}
}
