#ifndef QUERY_QUERY_HPP
#define QUERY_QUERY_HPP

#include "matador/sql/query_intermediates.hpp"

namespace matador::sql {

class connection;

class query
{
public:
  explicit query(connection &db, const sql::schema &schema);
  query(const query &) = delete;
  query& operator=(const query &) = delete;

  query_create_intermediate create();
  query_drop_intermediate drop();
  query_select_intermediate select(std::initializer_list<column> columns);
  query_select_intermediate select(const std::vector<column>& columns);
  query_select_intermediate select(std::vector<column> columns, std::initializer_list<column> additional_columns);
  template<class Type>
  query_select_intermediate select() {
    return select(column_generator::generate<Type>(schema_));
  }
  query_insert_intermediate insert();
  query_update_intermediate update(const sql::table &table);
  query_delete_intermediate remove();

private:
  connection &connection_;
  const sql::schema &schema_;
};

}
#endif //QUERY_QUERY_HPP
