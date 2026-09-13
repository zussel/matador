#ifndef QUERY_SET_INTERMEDIATE_HPP
#define QUERY_SET_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

#include "matador/query/criteria/abstract_criteria.hpp"

#include "matador/query/intermediates/execute_where_intermediate.hpp"

namespace matador::query {
class set_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  execute_where_intermediate where(std::unique_ptr<abstract_criteria> cond) const;

private:
  execute_where_intermediate where_clause(std::unique_ptr<abstract_criteria> &&cond) const;
};
}

#endif //QUERY_SET_INTERMEDIATE_HPP
