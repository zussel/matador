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

  [[nodiscard]] query_create_intermediate create() const;
  [[nodiscard]] query_drop_intermediate drop() const;
  [[nodiscard]] query_select_intermediate select(std::initializer_list<column> columns) const;
  [[nodiscard]] query_select_intermediate select(const std::vector<column>& columns) const;
  [[nodiscard]] query_select_intermediate select(const std::vector<std::string> &column_names) const;
  [[nodiscard]] query_select_intermediate select(std::vector<column> columns, std::initializer_list<column> additional_columns) const;
  template<class Type>
  [[nodiscard]] query_select_intermediate select() const {
    return select(column_generator::generate<Type>(schema_));
  }
  [[nodiscard]] query_insert_intermediate insert() const;
  [[nodiscard]] query_update_intermediate update(const sql::table &table) const;
  [[nodiscard]] query_delete_intermediate remove() const;

private:
  connection &connection_;
  const sql::schema &schema_;
};

}
#endif //QUERY_QUERY_HPP
