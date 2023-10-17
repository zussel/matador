#include "matador/db/mssql/mssql_dialect_linker.hpp"

#include "matador/sql/dialect_token.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

namespace mssql {

void mssql_dialect_linker::visit(const matador::detail::top &limit)
{
  std::stringstream res;
  res << token_string(limit.type) << " (" << limit.limit_ << ") ";
  append_to_result(*top().dialect, res.str());
}

}

}