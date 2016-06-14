//
// Created by sascha on 6/13/16.
//

#include "mssql_dialect_compiler.hpp"

namespace oos {

namespace mssql {

mssql_dialect_compiler::mssql_dialect_compiler()
{ }

void mssql_dialect_compiler::visit(const oos::detail::select &select1)
{
  current_select_ = current_;
}

void mssql_dialect_compiler::visit(const oos::detail::top &top1)
{
  if (current_select_ == tokens_.end()) {
    return;
  }

  // move limit behind select

  current_select_ = tokens_.end();
}

void mssql_dialect_compiler::on_compile_start()
{
  current_select_ = tokens_.end();
}

}
}