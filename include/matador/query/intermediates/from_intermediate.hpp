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

  join_intermediate join_left(const table &t);
  join_intermediate join_left(const query_context &ctx);
  from_intermediate join_left(join_data &data);
  from_intermediate join_left(std::vector<join_data> &data_vector);

  where_intermediate where(std::unique_ptr<abstract_criteria> &&cond);
  group_by_intermediate group_by(const column &column);
  group_by_intermediate group_by(std::initializer_list<column> columns);
  order_by_intermediate order_by(const column &col);
  order_by_intermediate order_by(std::initializer_list<column> columns);

private:
  where_intermediate where_clause(std::unique_ptr<abstract_criteria> &&cond);
};
}

#endif //QUERY_FROM_INTERMEDIATE_HPP
