#ifndef QUERY_QUERY_DATA_HPP
#define QUERY_QUERY_DATA_HPP

#include "matador/sql/query_part.hpp"
#include "matador/sql/table.hpp"

#include <memory>
#include <vector>

namespace matador::sql {

class connection;
class query_part;
class dialect;
class schema;

struct query_compile_context
{
  // explicit query_compile_context(connection &db, const sql::schema &schema)
  // : db(db)
  // , schema(schema) {}
  // connection &db;
  // const sql::schema &schema;
  std::vector<std::unique_ptr<query_part>> parts{};
  std::vector<column_definition> columns{};
  std::unordered_map<std::string, table> tables{};
};

}

#endif //QUERY_QUERY_DATA_HPP
