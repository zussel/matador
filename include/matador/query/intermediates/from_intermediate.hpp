#ifndef QUERY_FROM_INTERMEDIATE_HPP
#define QUERY_FROM_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"
#include "matador/query/join_data.hpp"
#include "matador/query/query_context.hpp"

#include "matador/query/intermediates/where_intermediate.hpp"

namespace matador::query {
class join_intermediate;

class from_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  /**
   * Appends a left join for a table and transitions to the join state.
   *
   * @code{.cpp}
   * auto query = select().from(customers).join_left(orders).on("customer_id"_col == "id"_col);
   * @endcode
   */
  join_intermediate join_left(const table &t) const;
  /** Appends a left join for a compiled subquery and transitions to the join state. */
  join_intermediate join_left(const query_context &ctx) const;
  /** Appends a table join and its ON condition, then remains in the from state. */
  from_intermediate join_left(join_data &data) const;
  /** Appends every table join and ON condition, then remains in the from state. */
  from_intermediate join_left(std::vector<join_data> &data_vector) const;

  /**
   * Appends a WHERE clause.
   *
   * @param cond Condition ownership is transferred to the query.
   * @return A fetchable where state supporting grouping and ordering.
   *
   * @code{.cpp}
   * auto query = select().from(customers).where("id"_col == 42);
   * @endcode
   */
  where_intermediate where(std::unique_ptr<abstract_criteria> &&cond) const;
  /**
   * Appends one GROUP BY column and transitions to the group-by state.
   *
   * @code{.cpp}
   * auto query = select({"country"_col}).from(customers).group_by("country"_col);
   * @endcode
   */
  group_by_intermediate group_by(const column &column) const;
  /** Appends GROUP BY columns and transitions to the group-by state. */
  group_by_intermediate group_by(std::initializer_list<column> columns) const;
  /**
   * Appends one ORDER BY column and transitions to the order-by state.
   *
   * @code{.cpp}
   * auto query = select().from(customers).order_by("id"_col).desc().limit(10).offset(20);
   * @endcode
   */
  order_by_intermediate order_by(const column &col) const;
  /** Appends ORDER BY columns and transitions to the order-by state. */
  order_by_intermediate order_by(std::initializer_list<column> columns) const;

private:
  where_intermediate where_clause(std::unique_ptr<abstract_criteria> &&cond) const;
};
}

#endif //QUERY_FROM_INTERMEDIATE_HPP
