#include "matador/query/intermediates/select_intermediate.hpp"

#include "matador/query/intermediates/join_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
select_intermediate::select_intermediate(const std::vector<column> &columns) {
  context_->parts.push_back(std::make_shared<internal::query_select_part>(columns));
}

fetchable_query select_intermediate::nextval(const std::string &sequence_name) const {
  auto context = clone_context();
  context->parts.clear();
  context->parts.push_back(std::make_shared<internal::query_select_nextval_part>(sequence_name));
  return {context};
}

fetchable_query select_intermediate::currval(const std::string &sequence_name) const {
  auto context = clone_context();
  context->parts.clear();
  context->parts.push_back(std::make_shared<internal::query_select_currval_part>(sequence_name));
  return {context};
}

from_intermediate select_intermediate::from(const std::vector<table> &tables) const {
  if (tables.empty()) {
    throw std::invalid_argument("SELECT requires at least one table in FROM");
  }
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_from_part>(tables));
  return {context};
}
} // namespace matador::query
