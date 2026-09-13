#ifndef QUERY_ORDER_DIRECTION_INTERMEDIATE_HPP
#define QUERY_ORDER_DIRECTION_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class limit_intermediate;

class order_direction_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  /** Appends a LIMIT clause and transitions to the fetchable limit state. */
  limit_intermediate limit(size_t limit) const;
};
}

#endif //QUERY_ORDER_DIRECTION_INTERMEDIATE_HPP
