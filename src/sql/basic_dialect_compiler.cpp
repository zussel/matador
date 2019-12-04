//
// Created by sascha on 6/9/16.
//

#include <iostream>
#include "matador/sql/sql.hpp"
#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

namespace detail {

void basic_dialect_compiler::compile(basic_dialect &dialect)
{
  on_compile_start();

  auto last = dialect.top().tokens_.end();
  token_list_t::iterator next;

  while (dialect.top().current != last) {
    (next = dialect.top().current)++;
    (*dialect.top().current)->accept(*this);
      dialect.top().current = next;
  }

  on_compile_finish();
}

void basic_dialect_compiler::visit(const matador::detail::create &) { }

void basic_dialect_compiler::visit(const matador::detail::drop &) { }

void basic_dialect_compiler::visit(const matador::detail::select &) { }

void basic_dialect_compiler::visit(const matador::detail::distinct &) { }

void basic_dialect_compiler::visit(const matador::detail::update &) { }

void basic_dialect_compiler::visit(const matador::detail::tablename &) { }

void basic_dialect_compiler::visit(const matador::detail::set &) { }

void basic_dialect_compiler::visit(const matador::columns &) { }

void basic_dialect_compiler::visit(const matador::column &) { }

void basic_dialect_compiler::visit(const matador::detail::typed_column &) { }

void basic_dialect_compiler::visit(const matador::detail::typed_identifier_column &) { }

void basic_dialect_compiler::visit(const matador::detail::typed_varchar_column &) { }

void basic_dialect_compiler::visit(const matador::detail::identifier_varchar_column &) { }

void basic_dialect_compiler::visit(const matador::detail::basic_value_column &) { }

void basic_dialect_compiler::visit(const matador::detail::from &) { }

void basic_dialect_compiler::visit(const matador::detail::where &) { }

void basic_dialect_compiler::visit(const matador::detail::basic_condition &) { }

void basic_dialect_compiler::visit(const matador::detail::basic_column_condition &) { }

void basic_dialect_compiler::visit(const matador::detail::basic_in_condition &) { }

void basic_dialect_compiler::visit(const matador::detail::order_by &) { }

void basic_dialect_compiler::visit(const matador::detail::asc &) { }

void basic_dialect_compiler::visit(const matador::detail::desc &) { }

void basic_dialect_compiler::visit(const matador::detail::group_by &) { }

void basic_dialect_compiler::visit(const matador::detail::insert &) { }

void basic_dialect_compiler::visit(const matador::detail::values &) { }

void basic_dialect_compiler::visit(const matador::basic_value &) { }

void basic_dialect_compiler::visit(const matador::detail::remove &) { }

void basic_dialect_compiler::visit(const matador::detail::top &) { }

void basic_dialect_compiler::visit(const matador::detail::as &) { }

void basic_dialect_compiler::visit(const matador::detail::begin &) { }

void basic_dialect_compiler::visit(const matador::detail::commit &) { }

void basic_dialect_compiler::visit(const matador::detail::rollback &) { }

void basic_dialect_compiler::visit(matador::detail::query &) {}

void basic_dialect_compiler::on_compile_start() { }

void basic_dialect_compiler::on_compile_finish() { }

basic_dialect &basic_dialect_compiler::dialect() const
{
  return *dialect_;
}

build_info &basic_dialect_compiler::top() const
{
  return dialect_->top();
}

void basic_dialect_compiler::dialect(basic_dialect *d)
{
  dialect_ = d;
}

}

}
