#include "matador/query/internal/column_value_pair.hpp"

#include <utility>

namespace matador::query::internal {
column_value_pair::column_value_pair(std::string column_name, column_expression_ptr expression)
: column_name_(std::move(column_name))
, expression_(std::move(expression)){
}

const std::string &column_value_pair::column_name() const {
  return column_name_;
}

const abstract_column_expression& column_value_pair::expression() const {
  return *expression_;
}

bool operator==( const column_value_pair& lhs, const column_value_pair& rhs ) {
  return lhs.column_name_ == rhs.column_name_ && lhs.expression_ == rhs.expression_;
}

bool operator!=( const column_value_pair& lhs, const column_value_pair& rhs ) {
  using namespace matador::utils;
  return !(lhs == rhs);
}
}