#include "matador/query/intermediates/order_by_intermediate.hpp"
#include "matador/query/intermediates/order_direction_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
order_direction_intermediate order_by_intermediate::asc() {
  context_->parts.push_back(std::make_unique<internal::query_order_by_asc_part>());
  return {context_};
}

order_direction_intermediate order_by_intermediate::desc() {
  context_->parts.push_back(std::make_unique<internal::query_order_by_desc_part>());
  return {context_};
}
}
