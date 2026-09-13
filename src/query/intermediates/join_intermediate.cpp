#include "matador/query/intermediates/join_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
on_intermediate join_intermediate::on(std::unique_ptr<abstract_criteria> &&cond) const {
  return on_clause(std::move(cond));
}

on_intermediate join_intermediate::on_clause(std::unique_ptr<abstract_criteria> &&cond) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_on_part>(std::move(cond)));
  return {context};
}
}
