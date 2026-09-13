#ifndef QUERY_EXECUTE_ORDER_DIRECTION_INTERMEDIATE_HPP
#define QUERY_EXECUTE_ORDER_DIRECTION_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

namespace matador::query {
class execute_limit_intermediate;

class execute_order_direction_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  execute_limit_intermediate limit(size_t limit) const;
};
}

#endif //QUERY_EXECUTE_ORDER_DIRECTION_INTERMEDIATE_HPP
