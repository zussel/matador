#include "matador/query/expression/value_expression.hpp"
#include "matador/query/expression/expression_visitor.hpp"

namespace matador::query {
value_expression::value_expression(utils::database_type value)
: value_(std::move(value)) {
}

void value_expression::accept(expression_visitor &visitor) const {
  visitor.visit(*this);
}

const utils::database_type& value_expression::value() const {
  return value_;
}
}