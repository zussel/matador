#include "matador/query/intermediates/select_intermediate.hpp"

#include "matador/query/intermediates/join_intermediate.hpp"

#include "matador/query/builder/query_data.hpp"
#include "matador/query/internal/query_parts.hpp"

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

from_intermediate
select_intermediate::from(const std::initializer_list<std::string_view> tables) const {
  if (tables.empty()) {
    throw std::invalid_argument("SELECT requires at least one table in FROM");
  }

  std::vector<table> v;
  v.reserve(tables.size());

  for (const auto table_name : tables) {
    v.emplace_back(std::string{table_name});
  }

  return from(v);
}

from_intermediate select_intermediate::from(const std::vector<table> &tables) const {
  if (tables.empty()) {
    throw std::invalid_argument("SELECT requires at least one table in FROM");
  }
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_from_part>(tables));
  return {context};
}

table select_intermediate::to_table(const table &tab) {
  return tab;
}

table select_intermediate::to_table(table &&tab) {
  return std::move(tab);
}

table select_intermediate::to_table(const std::string &table_name) {
  return table{table_name};
}

table select_intermediate::to_table(std::string &&table_name) {
  return table{std::move(table_name)};
}

table select_intermediate::to_table(std::string_view table_name) {
  return table{std::string{table_name}};
}

table select_intermediate::to_table(const char *table_name) {
  return table{std::string{table_name}};
}
} // namespace matador::query
