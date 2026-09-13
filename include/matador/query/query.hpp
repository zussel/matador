#ifndef QUERY_QUERY_HPP
#define QUERY_QUERY_HPP

#include "matador/query/column.hpp"

#include "matador/query/intermediates/alter_intermediate.hpp"
#include "matador/query/intermediates/create_intermediate.hpp"
#include "matador/query/intermediates/delete_intermediate.hpp"
#include "matador/query/intermediates/drop_intermediate.hpp"
#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/insert_intermediate.hpp"
#include "matador/query/intermediates/join_intermediate.hpp"
#include "matador/query/intermediates/limit_intermediate.hpp"
#include "matador/query/intermediates/offset_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"
#include "matador/query/intermediates/order_direction_intermediate.hpp"
#include "matador/query/intermediates/select_intermediate.hpp"
#include "matador/query/intermediates/update_intermediate.hpp"
#include "matador/query/intermediates/where_intermediate.hpp"

// #include "matador/query/generator.hpp"

namespace matador::query {
//class schema;
column count(const std::string &column);
column count_all();
column sum(const std::string &column);
column avg(const std::string &column);
column maximum(const std::string &column);
column minimum(const std::string &column);

[[nodiscard]] create_intermediate create();
/**
 * Starts a DROP query.
 *
 * @return A drop state that can drop a table, sequence, or schema.
 *
 * @code{.cpp}
 * auto query = drop().table(customers);
 * @endcode
 */
[[nodiscard]] drop_intermediate drop();
/**
 * Starts a query that selects every column.
 *
 * @return A select state that can start a FROM clause or select a sequence value.
 *
 * @code{.cpp}
 * auto query = select().from(customers);
 * @endcode
 */
[[nodiscard]] select_intermediate select();
/**
 * Starts a query with the given projection.
 *
 * @param columns Columns to select.
 * @return A select state that can start a FROM clause or select a sequence value.
 *
 * @code{.cpp}
 * auto query = select({"id"_col, "name"_col}).from(customers);
 * @endcode
 */
[[nodiscard]] select_intermediate select(std::initializer_list<column> columns);
/** Starts a query with the given projection. See select(std::initializer_list<column>). */
[[nodiscard]] select_intermediate select(const std::vector<column>& columns);
/** Starts a query by converting every column name into a plain column. */
[[nodiscard]] select_intermediate select(const std::vector<std::string> &column_names);
//template<class Type>
//[[nodiscard]] query_select_intermediate select(const schema &scm) {
//  return select(generator::columns<Type>(scm));
//}
[[nodiscard]] insert_intermediate insert();
/**
 * Starts an UPDATE query for a table.
 *
 * @param table Target table.
 * @return An update state that accepts assignments and filters.
 *
 * @code{.cpp}
 * auto query = update(customers)
 *   .set("status"_col, column_expression{"active"})
 *   .where("id"_col == 42);
 * @endcode
 */
[[nodiscard]] update_intermediate update(const table &table);
/**
 * Starts a DELETE query.
 *
 * @return A delete state that accepts a source table.
 *
 * @code{.cpp}
 * auto query = remove().from(customers).where("id"_col == 42);
 * @endcode
 */
[[nodiscard]] delete_intermediate remove();

[[nodiscard]] alter_intermediate alter();
}
#endif //QUERY_QUERY_HPP
