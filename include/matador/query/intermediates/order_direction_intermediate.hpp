#ifndef QUERY_ORDER_DIRECTION_INTERMEDIATE_HPP
#define QUERY_ORDER_DIRECTION_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class limit_intermediate;

class order_direction_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  limit_intermediate limit(size_t limit);
};
}

#endif //QUERY_ORDER_DIRECTION_INTERMEDIATE_HPP
