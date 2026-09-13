#ifndef QUERY_ORDER_BY_INTERMEDIATE_HPP
#define QUERY_ORDER_BY_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"

namespace matador::query {
class order_direction_intermediate;

class order_by_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  /** Appends ascending sort direction and transitions to a fetchable order state. */
  order_direction_intermediate asc() const;
  /** Appends descending sort direction and transitions to a fetchable order state. */
  order_direction_intermediate desc() const;
};
}

#endif //QUERY_ORDER_BY_INTERMEDIATE_HPP
