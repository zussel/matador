#include "matador/query/intermediates/create_intermediate.hpp"

#include "matador/query/builder/query_data.hpp"
#include "matador/query/internal/query_parts.hpp"

#include <vector>

namespace matador::query {

create_intermediate::create_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_create_part>());
}
create_table_intermediate create_intermediate::table(const std::string &table_name) const {
  return this->table(query::table{table_name});
}

create_table_intermediate create_intermediate::table(const class table &tab) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_create_table_part>(tab));
  return {context};
}

query_create_sequence_intermediate create_intermediate::sequence(const std::string& sequence_name) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_create_sequence_part>(sequence_name));
  return {context};
}

executable_query create_intermediate::schema( const std::string& schema_name ) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_create_schema_part>(schema_name));
  return {context};
}

executable_query create_table_columns_intermediate::constraints(const std::initializer_list<constraint> constraints) const {
  return this->constraints(std::list(constraints));
}

executable_query create_table_columns_intermediate::constraints(const std::list<constraint>& constraints) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_create_table_constraints_part>(constraints));
  return {context};
}

create_table_columns_intermediate create_table_intermediate::columns(const std::initializer_list<column> cols) const {
  return columns(std::vector(cols));
}

create_table_columns_intermediate create_table_intermediate::columns(const std::list<column> &cols) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_create_table_columns_part>(cols));
  return {context};
}

create_table_columns_intermediate create_table_intermediate::columns(const std::vector<column>& cols) const {
  std::list<column> columns;
  for (const auto& col : cols) {
    columns.emplace_back(col);
  }
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_create_table_columns_part>(columns));
  return {context};
}
}
