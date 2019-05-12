//
// Created by sascha on 6/13/16.
//

#include "matador/db/mssql/mssql_dialect_compiler.hpp"

#include "matador/sql/basic_dialect.hpp"

namespace matador {

namespace mssql {

void mssql_dialect_compiler::visit(const matador::detail::select &)
{
  commands_.push(top().current);
}

void mssql_dialect_compiler::visit(const matador::detail::update &)
{
  commands_.push(top().current);
}

void mssql_dialect_compiler::visit(const matador::detail::remove &)
{
  commands_.push(top().current);
}

void mssql_dialect_compiler::visit(const matador::detail::top &)
{
  if (commands_.empty()) {
    return;
  }

  // move limit behind select
  auto limit = *top().current;
  top().tokens_.erase(top().current);
  auto current_command = commands_.top();
  commands_.pop();
  top().tokens_.insert(++current_command, limit);
}

void mssql_dialect_compiler::on_compile_start()
{
  while (!commands_.empty()) {
    commands_.pop();
  }
}

}
}