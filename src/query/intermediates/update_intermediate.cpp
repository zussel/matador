#include "matador/query/intermediates/update_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
update_intermediate::update_intermediate(const table &tab) {
  context_->parts.push_back(std::make_unique<internal::query_update_part>(tab));
}

update_intermediate update_intermediate::set(const column &col, column_expression &&expression) const {
  auto context = clone_context();
  std::vector<internal::column_value_pair> key_value_pairs;
  key_value_pairs.emplace_back(col, std::move(expression));
  context->parts.push_back(std::make_shared<internal::query_set_part>(std::move(key_value_pairs)));
  return {context};
}

execute_where_intermediate update_intermediate::where(std::unique_ptr<abstract_criteria> cond) const {
  return where_clause(std::move(cond));
}

execute_where_intermediate update_intermediate::where_clause(std::unique_ptr<abstract_criteria> &&cond) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_where_part>(std::move(cond)));
  return {context};
}
}
