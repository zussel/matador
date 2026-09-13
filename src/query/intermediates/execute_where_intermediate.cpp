#include "matador/query/intermediates/execute_where_intermediate.hpp"

#include "matador/query/intermediates/execute_limit_intermediate.hpp"
#include "matador/query/intermediates/execute_order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"

#include "matador/query/query_data.hpp"

namespace matador::query {
fetchable_query execute_where_intermediate::returning(const std::vector<column> &table_columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_returning_part>(table_columns));
  return {context};
}

execute_limit_intermediate execute_where_intermediate::limit(size_t limit) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_limit_part>(limit));
  return {context};
}

execute_order_by_intermediate execute_where_intermediate::order_by(const column &col) const {
  return order_by({col});
}

execute_order_by_intermediate execute_where_intermediate::order_by(std::initializer_list<column> columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_part>(columns));
  return {context};
}
}
