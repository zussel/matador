#ifndef QUERY_QUERY_CONTEXT_HPP
#define QUERY_QUERY_CONTEXT_HPP

#include "matador/sql/column_definition.hpp"
#include "matador/sql/table.hpp"

namespace matador::sql {

struct query_context
{
  std::string sql;
  std::string command_name;
  sql::table table{""};
  std::vector<column_definition> prototype;
  std::vector<std::string> result_vars;
  std::vector<std::string> bind_vars;

  std::unordered_map<std::string, std::string> column_aliases;
  std::unordered_map<std::string, std::string> table_aliases;
};

}

#endif //QUERY_QUERY_CONTEXT_HPP
