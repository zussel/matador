#include "matador/query/intermediates/where_intermediate.hpp"
#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
group_by_intermediate where_intermediate::group_by(const column &column) const {
  return group_by({column});
}

group_by_intermediate where_intermediate::group_by(std::initializer_list<column> columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_group_by_part>(columns));
  return {context};
}

order_by_intermediate where_intermediate::order_by(const column &col) const {
  return order_by({col});
}

order_by_intermediate where_intermediate::order_by(std::initializer_list<column> columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_part>(columns));
  return {context};
}
}
