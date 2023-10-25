#include "matador/db/mssql/mssql_dialect_compiler.hpp"

#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/sql.hpp"

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
  auto limit = top().erase_current();
  top().insert_after(++commands_.top(), std::move(limit));
  commands_.pop();
}

void mssql_dialect_compiler::on_compile_start()
{
  while (!commands_.empty()) {
    commands_.pop();
  }
}

}
}