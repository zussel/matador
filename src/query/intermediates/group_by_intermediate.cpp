#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
order_by_intermediate group_by_intermediate::order_by(const column &col) {
  return order_by({col});
}

order_by_intermediate group_by_intermediate::order_by(std::initializer_list<column> columns) {
  context_->parts.push_back(std::make_unique<internal::query_order_by_part>(columns));
  return {context_};
}
}
