#ifndef QUERY_UPDATE_INTERMEDIATE_HPP
#define QUERY_UPDATE_INTERMEDIATE_HPP

#include "matador/query/intermediates/execute_where_intermediate.hpp"

#include "matador/query/criteria/abstract_criteria.hpp"
#include "matador/query/expression/column_expression.hpp"
// #include "matador/query/generator.hpp"

#include "matador/query/internal/column_value_pair.hpp"

namespace matador::query {
namespace internal {
class query_set_part;
}

class update_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  /**
   * Starts an UPDATE query for a table.
   *
   * @code{.cpp}
   * auto query = update(customers)
   *   .set("status"_col, column_expression{"active"})
   *   .where("id"_col == 42);
   * @endcode
   */
  explicit update_intermediate(const table &tab);

  /** Adds an assignment without changing states previously derived from this update. */
  update_intermediate set(const column &col, column_expression &&expression) const;
  // template<class Type>
  // query_set_intermediate set(const Type &obj) {
  // return set(generator::column_value_pairs(obj));
  // }
  // template<class Type>
  // query_set_intermediate set() {
  // return set(generator::column_value_pairs<Type>());
  // }
  /** Appends a WHERE clause without changing states previously derived from this update. */
  execute_where_intermediate where(std::unique_ptr<abstract_criteria> cond) const;

private:
  execute_where_intermediate where_clause(std::unique_ptr<abstract_criteria> &&cond) const;
};
}

#endif //QUERY_UPDATE_INTERMEDIATE_HPP
