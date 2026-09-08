#ifndef QUERY_QUERY_HPP
#define QUERY_QUERY_HPP

#include "matador/query/column.hpp"

#include "matador/query/intermediates/alter_intermediate.hpp"
#include "matador/query/intermediates/create_intermediate.hpp"

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
// [[nodiscard]] query_drop_intermediate drop();
// [[nodiscard]] query_select_intermediate select();
// [[nodiscard]] query_select_intermediate select(std::initializer_list<table_column> columns);
// [[nodiscard]] query_select_intermediate select(const std::vector<table_column>& columns);
// [[nodiscard]] query_select_intermediate select(const std::vector<std::string> &column_names);
//template<class Type>
//[[nodiscard]] query_select_intermediate select(const schema &scm) {
//  return select(generator::columns<Type>(scm));
//}
// [[nodiscard]] query_insert_intermediate insert();
// [[nodiscard]] query_update_intermediate update(const table &table);
// [[nodiscard]] query_delete_intermediate remove();

[[nodiscard]] alter_intermediate alter();
}
#endif //QUERY_QUERY_HPP
