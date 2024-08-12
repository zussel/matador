#ifndef QUERY_QUERY_DATA_HPP
#define QUERY_QUERY_DATA_HPP

#include "matador/sql/query_part.hpp"
#include "matador/sql/table.hpp"

#include <memory>
#include <vector>

namespace matador::sql {

class query_part;

struct query_data
{
//  SqlCommands command;
  std::vector<std::unique_ptr<query_part>> parts;
  std::vector<column_definition> columns;
};

}

#endif //QUERY_QUERY_DATA_HPP
