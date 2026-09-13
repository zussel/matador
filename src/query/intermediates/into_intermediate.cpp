#include "matador/query/intermediates/into_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
values_intermediate into_intermediate::values(const std::initializer_list<std::variant<placeholder, utils::database_type>> values) const {
  return this->values(std::vector(values));
}

values_intermediate into_intermediate::values(std::vector<std::variant<placeholder, utils::database_type>> &&values) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_values_part>(std::move(values)));
  return {context};
}

values_intermediate into_intermediate::values(std::vector<placeholder>&& values) const {
  std::vector<std::variant<placeholder, utils::database_type>> transformed_values;
  transformed_values.reserve(values.size());
  for (auto&& val : values) {
    transformed_values.emplace_back(val);
  }
  return this->values(std::move(transformed_values));
}

values_intermediate into_intermediate::values(std::vector<utils::database_type>&& values) const {
  std::vector<std::variant<placeholder, utils::database_type>> transformed_values;
  transformed_values.reserve(values.size());
  for (auto&& val : values) {
    transformed_values.emplace_back(val);
  }
  return this->values(std::move(transformed_values));
}

} // namespace matador::query
