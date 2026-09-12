#ifndef QUERY_GROUP_BY_INTERMEDIATE_HPP
#define QUERY_GROUP_BY_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
class order_by_intermediate;

class group_by_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  order_by_intermediate order_by(const column &col);
  order_by_intermediate order_by(std::initializer_list<column> columns);
};
}

#endif //QUERY_GROUP_BY_INTERMEDIATE_HPP
