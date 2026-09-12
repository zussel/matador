#ifndef QUERY_ORDER_BY_INTERMEDIATE_HPP
#define QUERY_ORDER_BY_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"

namespace matador::query {
class order_direction_intermediate;

class order_by_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  order_direction_intermediate asc();
  order_direction_intermediate desc();
};
}

#endif //QUERY_ORDER_BY_INTERMEDIATE_HPP
