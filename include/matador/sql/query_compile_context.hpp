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

enum class sql_command {
  SQL_UNKNOWN,
  SQL_CREATE,
  SQL_UPDATE,
  SQL_INSERT,
  SQL_DELETE,
  SQL_SELECT,
  SQL_DROP,
  SQL_ALTER
};

struct query_compile_context
{
  sql_command command;
  std::vector<std::unique_ptr<query_part>> parts{};
  std::vector<column_definition> columns{};
  std::unordered_map<std::string, table> tables{};
};

}

#endif //QUERY_QUERY_DATA_HPP
