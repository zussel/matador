//
// Created by sascha on 6/9/16.
//

#include "sql/sql.hpp"
#include "sql/basic_dialect_compiler.hpp"

namespace oos {

namespace detail {

void basic_dialect_compiler::compile(const sql &s)
{
  token_list_t backup;
  std::copy(s.token_list_.begin(), s.token_list_.end(), backup.begin());

  current_ = backup.begin();
  auto last = backup.end();

  while (current_ != last) {
    (*current_)->accept(*this);
  }

  s.token_list_.swap(backup);
}

void basic_dialect_compiler::visit(const oos::detail::create &create1) { }

void basic_dialect_compiler::visit(const oos::detail::drop &drop1) { }

void basic_dialect_compiler::visit(const oos::detail::select &select1) { }

void basic_dialect_compiler::visit(const oos::detail::distinct &distinct1) { }

void basic_dialect_compiler::visit(const oos::detail::update &update1) { }

void basic_dialect_compiler::visit(const oos::detail::set &set1) { }

void basic_dialect_compiler::visit(const oos::columns &columns1) { }

void basic_dialect_compiler::visit(const oos::column &column1) { }

void basic_dialect_compiler::visit(const oos::detail::typed_column &typedColumn) { }

void basic_dialect_compiler::visit(const oos::detail::identifier_column &identifierColumn) { }

void basic_dialect_compiler::visit(const oos::detail::typed_varchar_column &varchar_column) { }

void basic_dialect_compiler::visit(const oos::detail::identifier_varchar_column &varchar_column) { }

void basic_dialect_compiler::visit(const oos::detail::basic_value_column &value_column) { }

void basic_dialect_compiler::visit(const oos::detail::from &from1) { }

void basic_dialect_compiler::visit(const oos::detail::where &where1) { }

void basic_dialect_compiler::visit(const oos::detail::basic_condition &condition) { }

void basic_dialect_compiler::visit(const oos::detail::basic_column_condition &condition) { }

void basic_dialect_compiler::visit(const oos::detail::basic_in_condition &condition) { }

void basic_dialect_compiler::visit(const oos::detail::order_by &by) { }

void basic_dialect_compiler::visit(const oos::detail::asc &asc1) { }

void basic_dialect_compiler::visit(const oos::detail::desc &desc1) { }

void basic_dialect_compiler::visit(const oos::detail::group_by &by) { }

void basic_dialect_compiler::visit(const oos::detail::insert &insert1) { }

void basic_dialect_compiler::visit(const oos::detail::values &values1) { }

void basic_dialect_compiler::visit(const oos::detail::basic_value &value) { }

void basic_dialect_compiler::visit(const oos::detail::remove &remove1) { }

void basic_dialect_compiler::visit(const oos::detail::top &top1) { }

void basic_dialect_compiler::visit(const oos::detail::as &as1) { }

void basic_dialect_compiler::visit(const oos::detail::begin &begin1) { }

void basic_dialect_compiler::visit(const oos::detail::commit &commit1) { }

void basic_dialect_compiler::visit(const oos::detail::rollback &rollback1) { }

void basic_dialect_compiler::visit(const oos::detail::query &query1) { }

}

}
