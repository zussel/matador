#ifndef QUERY_JOIN_INTERMEDIATE_HPP
#define QUERY_JOIN_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/intermediates/from_intermediate.hpp"

namespace matador::query {
using on_intermediate = from_intermediate;

class join_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  // template<class Condition>
  // query_on_intermediate on(const Condition &cond)
  // {
  //   return on_clause(std::make_unique<Condition>(std::move(cond)));
  // }
  on_intermediate on(std::unique_ptr<abstract_criteria> &&cond);

private:
  on_intermediate on_clause(std::unique_ptr<abstract_criteria> &&cond);
};
}

#endif //QUERY_JOIN_INTERMEDIATE_HPP
