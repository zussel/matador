#include "matador/query/criteria/like_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
like_criteria::like_criteria(const column& col, std::string pattern)
: abstract_column_criteria(col)
, pattern_(std::move(pattern)){}

void like_criteria::accept(criteria_visitor &visitor) const {
  visitor.visit(*this);
}

const std::string & like_criteria::pattern() const {
  return pattern_;
}
}
