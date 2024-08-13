#include "matador/sql/query.hpp"
#include "matador/sql/connection.hpp"

namespace matador::sql {

query::query(connection &db, const sql::schema &schema)
: connection_(db)
, schema_(schema)
{}

query_create_intermediate query::create()
{
  return query_create_intermediate(connection_, schema_);
}

query_drop_intermediate query::drop()
{
  return query_drop_intermediate{connection_, schema_};
}

query_select_intermediate query::select(std::initializer_list<column> columns)
{
  return select(std::vector<column>{columns});
}

query_select_intermediate query::select(const std::vector<column>& columns)
{
  return {connection_, schema_, columns};
}

query_select_intermediate query::select(const std::vector<std::string> &column_names)
{
  std::vector<column> columns;
  columns.reserve(column_names.size());
  for (const auto &col_name : column_names) {
    columns.emplace_back(col_name);
  }
  return select(columns);
}

query_select_intermediate query::select(std::vector<column> columns, std::initializer_list<column> additional_columns)
{
  for (const auto &col : additional_columns) {
    columns.push_back(col);
  }
  return {connection_, schema_, columns};
}

query_insert_intermediate query::insert()
{
  return query_insert_intermediate{connection_, schema_};
}

query_update_intermediate query::update(const sql::table &table)
{
  return query_update_intermediate{connection_, schema_, table};
}

query_delete_intermediate query::remove()
{
  return query_delete_intermediate{connection_, schema_};
}

}