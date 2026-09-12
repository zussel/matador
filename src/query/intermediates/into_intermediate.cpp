#include "matador/query/intermediates/query_into_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
query_values_intermediate query_into_intermediate::values(const std::initializer_list<std::variant<utils::placeholder, utils::database_type>> values) {
  return this->values(std::vector(values));
}

query_values_intermediate query_into_intermediate::values(std::vector<std::variant<utils::placeholder, utils::database_type>> &&values) {
  context_->parts.push_back(std::make_unique<internal::query_values_part>(std::move(values)));
  return {context_};
}

query_values_intermediate query_into_intermediate::values(std::vector<utils::placeholder>&& values) {
  std::vector<std::variant<utils::placeholder, utils::database_type>> transformed_values;
  transformed_values.reserve(values.size());
  for (auto&& val : values) {
    transformed_values.emplace_back(val);
  }
  return this->values(std::move(transformed_values));
}

query_values_intermediate query_into_intermediate::values(std::vector<utils::database_type>&& values) {
  std::vector<std::variant<utils::placeholder, utils::database_type>> transformed_values;
  transformed_values.reserve(values.size());
  for (auto&& val : values) {
    transformed_values.emplace_back(val);
  }
  return this->values(std::move(transformed_values));
}

} // namespace matador::query
