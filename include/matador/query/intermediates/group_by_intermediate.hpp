#ifndef QUERY_GROUP_BY_INTERMEDIATE_HPP
#define QUERY_GROUP_BY_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
class order_by_intermediate;

class group_by_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  /** Appends one ORDER BY column and transitions to the order-by state. */
  order_by_intermediate order_by(const column &col) const;
  /** Appends ORDER BY columns and transitions to the order-by state. */
  order_by_intermediate order_by(std::initializer_list<column> columns) const;
};
}

#endif //QUERY_GROUP_BY_INTERMEDIATE_HPP
