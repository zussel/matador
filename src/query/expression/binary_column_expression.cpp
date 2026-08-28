#include "matador/query/expression/binary_column_expression.hpp"
#include "matador/query/expression/expression_visitor.hpp"

namespace matador::query {
binary_column_expression::binary_column_expression(column_expression_ptr left_column, const binary_expression_operator operand, column_expression_ptr right_column)
: left_column_(std::move(left_column))
, operand_(operand)
, right_column_(std::move(right_column)) {
}

void binary_column_expression::accept(expression_visitor &visitor) const {
  visitor.visit(*this);
}

const column_expression_ptr& binary_column_expression::left_column() const {
  return left_column_;
}

binary_expression_operator binary_column_expression::operand() const {
  return operand_;
}

const column_expression_ptr& binary_column_expression::right_column() const {
  return right_column_;
}
}