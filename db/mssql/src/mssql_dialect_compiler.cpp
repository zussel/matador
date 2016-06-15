//
// Created by sascha on 6/13/16.
//

#include "mssql_dialect_compiler.hpp"

namespace oos {

namespace mssql {

mssql_dialect_compiler::mssql_dialect_compiler()
{ }

void mssql_dialect_compiler::visit(const oos::detail::select &)
{
  selects_.push(current_);
}

void mssql_dialect_compiler::visit(const oos::detail::top &)
{
  if (selects_.empty()) {
    return;
  }

  // move limit behind select
  auto top = *current_;
  current_ = tokens_.erase(current_);
  auto current_select = selects_.top();
  selects_.pop();
  tokens_.insert(++current_select, top);
}

void mssql_dialect_compiler::on_compile_start()
{
  while (!selects_.empty()) {
    selects_.pop();
  }
}

void mssql_dialect_compiler::visit(const oos::detail::query &query1)
{
  int i = 9;
}

}
}