#ifndef MATADOR_QUERY_DATA_HPP
#define MATADOR_QUERY_DATA_HPP

#include "matador/query/column.hpp"
#include "matador/query/table.hpp"
#include "matador/query/query_part.hpp"

#include <unordered_map>

namespace matador::query {
struct query_data {
  std::vector<std::unique_ptr<query_part>> parts{};
  std::vector<column> columns{};
  std::unordered_map<std::string, table> tables{};
};
}
#endif //MATADOR_QUERY_DATA_HPP
