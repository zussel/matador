//
// Created by sascha on 19.08.16.
//

#include "mssql_dialect_linker.hpp"

#include "sql/dialect_token.hpp"

namespace oos {

namespace mssql {

void mssql_dialect_linker::visit(const oos::detail::top &limit)
{
  std::stringstream res;
  res << token_string(limit.type) << " (" << limit.limit_ << ") ";
  append_to_result(*top().dialect, res.str());
}

}

}