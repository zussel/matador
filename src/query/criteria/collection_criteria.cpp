#include "matador/query/criteria/collection_criteria.hpp"

#include "matador/query/criteria/criteria_visitor.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
collection_criteria::collection_criteria(const column& col, const collection_operator operand_, std::vector<criteria_value> values )
: abstract_column_criteria(col)
, operand_(operand_)
, values_(std::move(values))
{}

collection_criteria::collection_criteria(const column& col, const collection_operator operand_, const std::initializer_list<criteria_value> values )
: abstract_column_criteria(col)
, operand_(operand_)
, values_(values)
{}

void collection_criteria::accept(criteria_visitor& visitor) const {
  visitor.visit(*this);
}

collection_operator collection_criteria::operand() const {
  return operand_;
}

const std::vector<criteria_value>& collection_criteria::values() const {
  return values_;
}

collection_query_criteria::collection_query_criteria(const column& col, const collection_operator operand_, const query_context& ctx)
: abstract_column_criteria(col)
, operand_(operand_)
, query_context_(ctx){
}

void collection_query_criteria::accept(criteria_visitor &visitor) const {
  visitor.visit(*this);
}

collection_operator collection_query_criteria::operand() const {
  return operand_;
}

const query_context& collection_query_criteria::query() const {
  return query_context_;
}
}
