#include "matador/query/criteria/check_null_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"

namespace matador::query {
check_null_criteria::check_null_criteria(const column& col, const check_null_operator op)
: abstract_column_criteria(col)
, operator_{op} {}

void check_null_criteria::accept(criteria_visitor& visitor) const {
  visitor.visit(*this);
}
check_null_operator check_null_criteria::operand() const {
  return operator_;
}
}  // namespace matador::query