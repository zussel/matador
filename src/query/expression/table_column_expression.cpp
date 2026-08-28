#include "matador/query/expression/table_column_expression.hpp"
#include "matador/query/expression/expression_visitor.hpp"

namespace matador::query {
table_column_expression::table_column_expression(column col)
: column_(std::move(col)){
}

void table_column_expression::accept(expression_visitor &visitor) const {
    visitor.visit(*this);
}

const column& table_column_expression::col() const {
  return column_;
}
}