#include "matador/query/intermediates/alter_table_intermediate.hpp"

#include "matador/query/builder/query_data.hpp"
#include "matador/query/internal/query_parts.hpp"

namespace matador::query {
executable_query
query_add_foreign_key_constraint_intermediate::references(const std::string &table_name, const std::vector<column> &columns) const {
  return references(query::table{table_name}, columns);
}

executable_query query_add_foreign_key_constraint_intermediate::references( const table& tab, const std::vector<column>& columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_add_foreign_key_reference_part>(tab, columns));
  return {context};
}

add_primary_key_constraint_intermediate add_key_constraint_intermediate::primary_key(const column& col) const {
  return primary_keys({col});
}

add_primary_key_constraint_intermediate add_key_constraint_intermediate::primary_keys( const std::vector<column>& columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_add_primary_key_constraint_part>(columns));

  return {context};
}

query_add_foreign_key_constraint_intermediate add_key_constraint_intermediate::foreign_key(const column& col) const {
  return foreign_keys({col});
}

query_add_foreign_key_constraint_intermediate add_key_constraint_intermediate::foreign_keys(const std::vector<column>& columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_add_foreign_key_constraint_part>(columns));

  return {context};
}

executable_query alter_table_intermediate::add_constraint(const constraint &c) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::add_constraint_part_by_constraint>(c));

  return {context};
}

add_key_constraint_intermediate alter_table_intermediate::add_constraint(const std::string& name) const {
  auto context = clone_context();
    context->parts.push_back(std::make_unique<internal::query_add_key_constraint_part>(name));

    return {context};
}

executable_query alter_table_intermediate::drop_constraint(const constraint &c) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_drop_key_constraint_part_by_constraint>(c));
  return {context};
}

executable_query alter_table_intermediate::drop_constraint(const std::string& name) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_drop_key_constraint_part_by_name>(name));
  return {context};
}
}