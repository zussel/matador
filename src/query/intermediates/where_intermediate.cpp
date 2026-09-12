#include "matador/query/intermediates/where_intermediate.hpp"
#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
group_by_intermediate where_intermediate::group_by(const column &column) {
  return group_by({column});
}

group_by_intermediate where_intermediate::group_by(std::initializer_list<column> columns) {
  context_->parts.push_back(std::make_unique<internal::query_group_by_part>(columns));
  return {context_};
}

order_by_intermediate where_intermediate::order_by(const column &col) {
  return order_by({col});
}

order_by_intermediate where_intermediate::order_by(std::initializer_list<column> columns) {
  context_->parts.push_back(std::make_unique<internal::query_order_by_part>(columns));
  return {context_};
}
}
