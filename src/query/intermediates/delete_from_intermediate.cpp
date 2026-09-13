#include "matador/query/intermediates/delete_from_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
execute_where_intermediate delete_from_intermediate::where_clause(std::unique_ptr<abstract_criteria> &&cond) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_where_part>(std::move(cond)));
  return {context};
}

execute_where_intermediate delete_from_intermediate::where(std::unique_ptr<abstract_criteria> cond) const {
  return where_clause(std::move(cond));
}
}
