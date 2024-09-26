#include "matador/sql/query.hpp"

namespace matador::sql {

column alias(const std::string &column, const std::string &as)
{
  return sql::column{column, as};
}

column alias(column &&col, const std::string &as)
{
  col.as(as);
  return std::move(col);
}

column count(const std::string &column)
{
  return {sql_function_t::COUNT, column};
}

column count_all()
{
  return count("*");
}

query_create_intermediate query::create()
{
  return {};
}

query_drop_intermediate query::drop() {
  return {};
}

query_select_intermediate query::select( const std::initializer_list<column> columns) {
  return select(std::vector<column>{columns});
}

query_select_intermediate query::select(const std::vector<column>& columns) {
  return {columns};
}

query_select_intermediate query::select(const std::vector<std::string> &column_names) {
  std::vector<column> columns;
  columns.reserve(column_names.size());
  for (const auto &col_name : column_names) {
    columns.emplace_back(col_name);
  }
  return select(columns);
}

query_select_intermediate query::select(std::vector<column> columns, const std::initializer_list<column> additional_columns) {
  for (const auto &col : additional_columns) {
    columns.push_back(col);
  }
  return {columns};
}

query_insert_intermediate query::insert() {
  return {};
}

query_update_intermediate query::update(const sql::table &table) {
  return {table};
}

query_delete_intermediate query::remove() {
  return {};
}

}