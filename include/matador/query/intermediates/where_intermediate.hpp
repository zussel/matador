#ifndef QUERY_WHERE_INTERMEDIATE_HPP
#define QUERY_WHERE_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
class group_by_intermediate;
class order_by_intermediate;

class where_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  /** Appends one GROUP BY column and transitions to the group-by state. */
  group_by_intermediate group_by(const column &column) const;
  /** Appends GROUP BY columns and transitions to the group-by state. */
  group_by_intermediate group_by(std::initializer_list<column> columns) const;
  /** Appends one ORDER BY column and transitions to the order-by state. */
  order_by_intermediate order_by(const column &col) const;
  /** Appends ORDER BY columns and transitions to the order-by state. */
  order_by_intermediate order_by(std::initializer_list<column> columns) const;
};
}

#endif //QUERY_WHERE_INTERMEDIATE_HPP
