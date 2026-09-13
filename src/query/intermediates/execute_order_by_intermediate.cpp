#include "matador/query/intermediates/execute_order_by_intermediate.hpp"
#include "matador/query/intermediates/execute_order_direction_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
execute_order_direction_intermediate execute_order_by_intermediate::asc() const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_asc_part>());
  return {context};
}

execute_order_direction_intermediate execute_order_by_intermediate::desc() const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_desc_part>());
  return {context};
}
} // namespace matador::query
