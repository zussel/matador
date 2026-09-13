#include "matador/query/intermediates/drop_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"

#include "matador/query/query_data.hpp"

namespace matador::query {
drop_intermediate::drop_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_drop_part>());
}

executable_query drop_intermediate::table(const class table &tab) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_drop_table_part>(tab));
  return {context};
}

executable_query drop_intermediate::sequence(const std::string& sequence_name) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_drop_sequence_part>(sequence_name));
  return {context};
}

executable_query drop_intermediate::schema(const std::string& schema_name) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_drop_schema_part>(schema_name));
  return {context};
}
}
