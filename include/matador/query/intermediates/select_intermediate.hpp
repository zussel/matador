#ifndef QUERY_SELECT_INTERMEDIATE_HPP
#define QUERY_SELECT_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/intermediates/from_intermediate.hpp"

#include "matador/query/model/column.hpp"

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
  [[nodiscard]] fetchable_query nextval(const std::string& sequence_name) const;
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
  [[nodiscard]] fetchable_query currval(const std::string& sequence_name) const;

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
  [[nodiscard]] from_intermediate from(Tables&&... tables) const {
    if constexpr (sizeof...(tables) == 0) {
      throw std::invalid_argument("SELECT requires at least one table in FROM");
    }

    std::vector<table> v;
    v.reserve(sizeof...(tables));
    (v.emplace_back(to_table(std::forward<Tables>(tables))), ...);

    return from(v);
  }

  [[nodiscard]] from_intermediate from(std::initializer_list<std::string_view> tables) const;

private:
  [[nodiscard]] from_intermediate from(const std::vector<table>& tables) const;

  [[nodiscard]] static table to_table(const table& tab);
  [[nodiscard]] static table to_table(table&& tab);
  [[nodiscard]] static table to_table(const std::string& table_name);
  [[nodiscard]] static table to_table(std::string&& table_name);
  [[nodiscard]] static table to_table(std::string_view table_name);
  [[nodiscard]] static table to_table(const char* table_name);
};

}

#endif //QUERY_SELECT_INTERMEDIATE_HPP
