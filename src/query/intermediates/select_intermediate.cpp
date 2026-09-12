#include "matador/query/intermediates/select_intermediate.hpp"

#include "matador/query/intermediates/join_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
select_intermediate::select_intermediate(const std::vector<column> &columns) {
  context_->parts.push_back(std::make_unique<internal::query_select_part>(columns));
}

fetchable_query select_intermediate::nextval(const std::string &sequence_name) {
  context_->parts.pop_back();
  context_->parts.push_back(std::make_unique<internal::query_select_nextval_part>(sequence_name));
  return {context_};
}

fetchable_query select_intermediate::currval(const std::string &sequence_name) {
  context_->parts.pop_back();
  context_->parts.push_back(std::make_unique<internal::query_select_currval_part>(sequence_name));
  return {context_};
}

from_intermediate select_intermediate::from(const std::vector<table> &tables) {
  context_->parts.push_back(std::make_unique<internal::query_from_part>(tables));
  for (const auto &tab: tables) {
    context_->tables.insert({tab.name(), tab});
  }

  return {context_};
}
} // namespace matador::query
