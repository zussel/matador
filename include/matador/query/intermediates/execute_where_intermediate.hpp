#ifndef QUERY_EXECUTE_WHERE_INTERMEDIATE_HPP
#define QUERY_EXECUTE_WHERE_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"
#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class execute_limit_intermediate;
class execute_order_by_intermediate;

class execute_where_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  template<typename... TableColumns>
  fetchable_query returning(TableColumns... columns) const {
    const std::vector<column> tcv{std::forward<TableColumns>(columns)...};
    return returning(tcv);
  }

  execute_limit_intermediate limit(size_t limit) const;
  execute_order_by_intermediate order_by(const column &col) const;
  execute_order_by_intermediate order_by(std::initializer_list<column> columns) const;

private:
  fetchable_query returning(const std::vector<column> &table_columns) const;
};
}

#endif //QUERY_EXECUTE_WHERE_INTERMEDIATE_HPP
