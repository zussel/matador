#include "matador/query/criteria/logical_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"

namespace matador::query {
logical_criteria::logical_criteria(criteria_ptr left, const logical_operator op, criteria_ptr right)
: left_criteria_(std::move(left))
, operand_(op)
, right_criteria_(std::move(right))
{}

void logical_criteria::accept(criteria_visitor& visitor) const {
  visitor.visit(*this);
}

const criteria_ptr & logical_criteria::left_clause() const {
  return left_criteria_;
}

logical_operator logical_criteria::operand() const {
  return operand_;
}

const criteria_ptr & logical_criteria::right_clause() const {
  return right_criteria_;
}
}
