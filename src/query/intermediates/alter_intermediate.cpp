#include "matador/query/intermediates/alter_intermediate.hpp"

#include "matador/query/builder/query_data.hpp"
#include "matador/query/internal/query_parts.hpp"

namespace matador::query {
alter_intermediate::alter_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_alter_part>());
}
alter_table_intermediate alter_intermediate::table(const std::string &table_name) const {
  return this->table(query::table(table_name));
}

alter_table_intermediate alter_intermediate::table(const class table& tab) const {
  auto context = clone_context();
  context->parts.push_back(std::make_unique<internal::query_alter_table_part>(tab));
  return {context};
}
}