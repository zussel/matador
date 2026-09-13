#ifndef QUERY_DELETE_FROM_INTERMEDIATE_HPP
#define QUERY_DELETE_FROM_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

#include "matador/query/criteria/abstract_criteria.hpp"

#include "matador/query/intermediates/execute_where_intermediate.hpp"

namespace matador::query {
class delete_from_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  /** Appends a WHERE clause without changing other states derived from this delete query. */
  execute_where_intermediate where(std::unique_ptr<abstract_criteria> cond) const;

private:
  execute_where_intermediate where_clause(std::unique_ptr<abstract_criteria> &&cond) const;
};
}

#endif //QUERY_DELETE_FROM_INTERMEDIATE_HPP
