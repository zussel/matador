#include "matador/query/criteria/between_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"

namespace matador::query{
between_criteria::between_criteria(const column& col, const int64_t min, const int64_t max)
: abstract_column_criteria(col)
, min_(column_value{min})
, max_(column_value{max})
{}

between_criteria::between_criteria(const column& col, placeholder min, placeholder max)
: abstract_column_criteria(col)
, min_(min)
, max_(max)
{}

void between_criteria::accept( criteria_visitor& visitor ) const {
  visitor.visit(*this);
}

const criteria_value &between_criteria::minimum() const {
  return min_;
}

const criteria_value &between_criteria::maximum() const {
  return max_;
}
}
