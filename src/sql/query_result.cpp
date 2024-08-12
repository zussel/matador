#include "matador/sql/query_result.hpp"

#include "matador/sql/record.hpp"

namespace matador::sql::detail {

template<>
record *create_prototype<record>(const std::vector<column_definition> &prototype)
{
  auto result = std::make_unique<record>();
  for (const auto &col: prototype) {
    result->append({col.name(), col.type(), col.attributes().size(), col.index()});
  }
  return result.release();
}

}