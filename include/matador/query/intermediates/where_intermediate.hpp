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

  group_by_intermediate group_by(const column &column);
  group_by_intermediate group_by(std::initializer_list<column> columns);
  order_by_intermediate order_by(const column &col);
  order_by_intermediate order_by(std::initializer_list<column> columns);
};
}

#endif //QUERY_WHERE_INTERMEDIATE_HPP
