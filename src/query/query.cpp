#include "matador/query/query.hpp"
#include "matador/query/query_functions.hpp"

namespace matador::query {
column count(const std::string& column) {
  return column::make_query_function(query_functions::Count, column);
}

column count_all() {
  return count("*");
}

column sum(const std::string& column) {
  return column::make_query_function(query_functions::Sum, column);
}

column avg(const std::string& column) {
  return column::make_query_function(query_functions::Avg, column);
}

column maximum(const std::string& column) {
  return column::make_query_function(query_functions::Max, column);
}

column minimum(const std::string& column) {
  return column::make_query_function(query_functions::Min, column);
}

create_intermediate create() {
  return {};
}

//
// query_drop_intermediate drop() {
//   return {};
// }
//
select_intermediate select() {
  return select_intermediate{{}};
}

select_intermediate select(const std::initializer_list<column> columns) {
  return select(std::vector<column>{columns});
}

select_intermediate select(const std::vector<column>& columns) {
  return select_intermediate{columns};
}

select_intermediate select(const std::vector<std::string>& column_names) {
  std::vector<column> columns;
  columns.reserve(column_names.size());
  for (const auto& col_name : column_names) {
    columns.emplace_back(column::make_plain(col_name));
  }
  return select(columns);
}
//
// query_insert_intermediate insert() {
//   return {};
// }
//
// query_update_intermediate update(const table& table) {
//   return query_update_intermediate{table};
// }
//
// query_delete_intermediate remove() {
//   return {};
// }
//
alter_intermediate alter() {
  return {};
}
}
