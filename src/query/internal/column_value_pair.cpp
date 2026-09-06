#include "matador/query/internal/column_value_pair.hpp"

#include <utility>

namespace matador::query::internal {
column_value_pair::column_value_pair(column col, column_expression_ptr expression)
: column_(std::move(col))
, expression_(std::move(expression)){
}

const column &column_value_pair::col() const {
  return column_;
}

const abstract_column_expression& column_value_pair::expression() const {
  return *expression_;
}

bool operator==( const column_value_pair& lhs, const column_value_pair& rhs ) {
  return lhs.column_.equals(rhs.column_) && lhs.expression_ == rhs.expression_;
}

bool operator!=( const column_value_pair& lhs, const column_value_pair& rhs ) {
  using namespace matador::utils;
  return !(lhs == rhs);
}
}