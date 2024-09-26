#ifndef QUERY_QUERY_HPP
#define QUERY_QUERY_HPP

#include "matador/sql/query_intermediates.hpp"

namespace matador::sql {

class connection;

column alias(const std::string &column, const std::string &as);
column alias(column &&col, const std::string &as);
column count(const std::string &column);
column count_all();

class query
{
public:
  [[nodiscard]] static query_create_intermediate create();
  [[nodiscard]] static query_drop_intermediate drop();
  [[nodiscard]] static query_select_intermediate select(std::initializer_list<column> columns);
  [[nodiscard]] static query_select_intermediate select(const std::vector<column>& columns);
  [[nodiscard]] static query_select_intermediate select(const std::vector<std::string> &column_names);
  [[nodiscard]] static query_select_intermediate select(std::vector<column> columns, std::initializer_list<column> additional_columns);
  template<class Type>
  [[nodiscard]] static query_select_intermediate select(const sql::schema &schema) {
    return select(column_generator::generate<Type>(schema));
  }
  [[nodiscard]] static query_insert_intermediate insert();
  [[nodiscard]] static query_update_intermediate update(const sql::table &table);
  [[nodiscard]] static query_delete_intermediate remove();
};

}
#endif //QUERY_QUERY_HPP
