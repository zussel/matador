#include "matador/query/intermediates/alter_table_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
executable_query query_add_foreign_key_constraint_intermediate::references( const table& tab, const std::vector<column>& columns) {
  context_->parts.push_back(std::make_unique<internal::query_add_foreign_key_reference_part>(tab, columns));

  return {context_};
}

add_primary_key_constraint_intermediate add_key_constraint_intermediate::primary_key(const column& col) const {
  return primary_keys({col});
}

add_primary_key_constraint_intermediate add_key_constraint_intermediate::primary_keys( const std::vector<column>& columns) const {
  context_->parts.push_back(std::make_unique<internal::query_add_primary_key_constraint_part>(columns));

  return {context_};
}

query_add_foreign_key_constraint_intermediate add_key_constraint_intermediate::foreign_key(const column& col) const {
  return foreign_keys({col});
}

query_add_foreign_key_constraint_intermediate add_key_constraint_intermediate::foreign_keys(const std::vector<column>& columns) const {
  context_->parts.push_back(std::make_unique<internal::query_add_foreign_key_constraint_part>(columns));

  return {context_};
}

executable_query alter_table_intermediate::add_constraint(const constraint &c) {
  context_->parts.push_back(std::make_unique<internal::add_constraint_part_by_constraint>(c));

  return {context_};
}

add_key_constraint_intermediate alter_table_intermediate::add_constraint(const std::string& name) {
    context_->parts.push_back(std::make_unique<internal::query_add_key_constraint_part>(name));

    return {context_};
}

executable_query alter_table_intermediate::drop_constraint(const constraint &c) {
  context_->parts.push_back(std::make_unique<internal::query_drop_key_constraint_part_by_constraint>(c));
  return {context_};
}

executable_query alter_table_intermediate::drop_constraint(const std::string& name) {
    context_->parts.push_back(std::make_unique<internal::query_drop_key_constraint_part_by_name>(name));
    return {context_};
}
}