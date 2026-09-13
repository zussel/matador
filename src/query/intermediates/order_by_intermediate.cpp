#include "matador/query/intermediates/order_by_intermediate.hpp"
#include "matador/query/intermediates/order_direction_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
order_direction_intermediate order_by_intermediate::asc() const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_asc_part>());
  return {context};
}

order_direction_intermediate order_by_intermediate::desc() const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_desc_part>());
  return {context};
}
}
