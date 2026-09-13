#include "matador/query/intermediates/execute_order_direction_intermediate.hpp"
#include "matador/query/intermediates/execute_limit_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
execute_limit_intermediate execute_order_direction_intermediate::limit(size_t limit) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_limit_part>(limit));
  return {context};
}
}
