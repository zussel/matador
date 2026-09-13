#ifndef QUERY_EXECUTE_ORDER_BY_INTERMEDIATE_HPP
#define QUERY_EXECUTE_ORDER_BY_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"

namespace matador::query {
class execute_order_direction_intermediate;

class execute_order_by_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  execute_order_direction_intermediate asc() const;
  execute_order_direction_intermediate desc() const;
};
}

#endif //QUERY_EXECUTE_ORDER_BY_INTERMEDIATE_HPP
