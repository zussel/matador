#ifndef MATADOR_QUERY_DATA_HPP
#define MATADOR_QUERY_DATA_HPP

#include "matador/query/column.hpp"
#include "matador/query/table.hpp"
#include "matador/query/query_part.hpp"

#include <memory>
#include <vector>

namespace matador::query {
struct query_data {
  std::vector<std::shared_ptr<query_part>> parts{};
};
}
#endif //MATADOR_QUERY_DATA_HPP
