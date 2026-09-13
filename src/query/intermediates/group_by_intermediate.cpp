#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
order_by_intermediate group_by_intermediate::order_by(const column &col) const {
  return order_by({col});
}

order_by_intermediate group_by_intermediate::order_by(std::initializer_list<column> columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_part>(columns));
  return {context};
}
}
