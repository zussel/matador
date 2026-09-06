#include "matador/query/criteria/not_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"

namespace matador::query {
not_criteria::not_criteria(criteria_ptr clause)
: criteria_(std::move(clause))
{}

void not_criteria::accept(criteria_visitor& visitor) const {
  visitor.visit(*this);
}

const criteria_ptr & not_criteria::clause() const {
  return criteria_;
}
}
