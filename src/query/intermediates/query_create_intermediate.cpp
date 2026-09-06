#include "matador/query/intermediates/query_create_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

#include <vector>

namespace matador::query {

query_create_intermediate::query_create_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_create_part>());
}

query_create_table_intermediate query_create_intermediate::table(const class table &tab) {
  context_->parts.push_back(std::make_unique<internal::query_create_table_part>(tab));
  return {context_};
}

query_create_sequence_intermediate query_create_intermediate::sequence(const std::string& sequence_name) {
  context_->parts.push_back(std::make_unique<internal::query_create_sequence_part>(sequence_name));
  return {context_};
}

executable_query query_create_intermediate::schema( const std::string& schema_name ) {
  context_->parts.push_back(std::make_unique<internal::query_create_schema_part>(schema_name));
  return {context_};
}

executable_query query_create_table_columns_intermediate::constraints(const std::initializer_list<constraint> constraints) {
  return this->constraints(std::list(constraints));
}

executable_query query_create_table_columns_intermediate::constraints(const std::list<constraint>& constraints){
  context_->parts.push_back(std::make_unique<internal::query_create_table_constraints_part>(constraints));
  return {context_};
}

query_create_table_columns_intermediate query_create_table_intermediate::columns(const std::initializer_list<column> cols) {
  return columns(std::vector(cols));
}

query_create_table_columns_intermediate query_create_table_intermediate::columns(const std::list<column> &cols) {
  context_->parts.push_back(std::make_unique<internal::query_create_table_columns_part>(cols));
  return {context_};
}

query_create_table_columns_intermediate query_create_table_intermediate::columns(const std::vector<column>& cols) {
  std::list<column> columns;
  for (const auto& col : cols) {
    columns.emplace_back(col);
  }
  context_->parts.push_back(std::make_unique<internal::query_create_table_columns_part>(columns));
  return {context_};
}
}
