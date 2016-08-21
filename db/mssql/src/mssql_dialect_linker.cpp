//
// Created by sascha on 19.08.16.
//

#include "mssql_dialect_linker.hpp"

#include "sql/dialect_token.hpp"

namespace oos {

namespace mssql {

void mssql_dialect_linker::visit(const oos::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " (" << top.limit_ << ") ";
  build_info_->dialect->append_to_result(res.str());
}

}

}