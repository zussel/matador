#ifndef QUERY_SELECT_INTERMEDIATE_HPP
#define QUERY_SELECT_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/intermediates/from_intermediate.hpp"

#include "matador/query/column.hpp"

#include <stdexcept>
#include <vector>

namespace matador::query {

class from_intermediate;

class select_intermediate : public intermediate
{
public:
  /** Creates a select state with the supplied projection. Prefer the select() factories. */
  explicit select_intermediate(const std::vector<column>& columns);

  /**
   * Creates a query that returns the next value of a sequence.
   *
   * @param sequence_name Sequence name interpreted as a SQL string literal.
   * @return A fetchable select query. This does not modify other states derived from this state.
   *
   * @code{.cpp}
   * auto query = select().nextval("customer_id_seq");
   * @endcode
   */
  fetchable_query nextval(const std::string& sequence_name) const;
  /**
   * Creates a query that returns the current value of a sequence.
   *
   * @param sequence_name Sequence name interpreted as a SQL string literal.
   * @return A fetchable select query. This does not modify other states derived from this state.
   *
   * @code{.cpp}
   * auto query = select().currval("customer_id_seq");
   * @endcode
   */
  fetchable_query currval(const std::string& sequence_name) const;

  /**
   * Appends a FROM clause and transitions to the fetchable from state.
   *
   * @param tables One or more source tables.
   * @return A from state supporting joins, filters, grouping, and ordering.
   * @throws std::invalid_argument if no table is provided.
   *
   * @code{.cpp}
   * auto query = select({"id"_col}).from(customers);
   * @endcode
   */
  template<typename... Tables>
  from_intermediate from(const Tables&... tables) const {
    if constexpr (sizeof...(tables) == 0) {
      throw std::invalid_argument("SELECT requires at least one table in FROM");
    }
    std::vector<table> v { tables... };
    return from(v);
  }

private:
  from_intermediate from(const std::vector<table>& tables) const;
};

}

#endif //QUERY_SELECT_INTERMEDIATE_HPP
