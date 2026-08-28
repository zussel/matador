#include "matador/query/expression/placeholder_expression.hpp"

#include "matador/query/expression/expression_visitor.hpp"

namespace matador::query {
void placeholder_expression::accept(expression_visitor &visitor) const {
  visitor.visit(*this);
}
}