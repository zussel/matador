//
// Created by sascha on 24.06.19.
//
#include "matador/sql/query.hpp"

#include "matador/db/postgresql/postgresql_dialect_compiler.hpp"

namespace matador {
namespace postgresql {

void postgresql_dialect_compiler::visit(const matador::detail::select &)
{
  is_update = false;
  is_delete = false;
  has_condition_column_name_ = false;
}

void postgresql_dialect_compiler::visit(const matador::detail::update &)
{
  is_update = true;
  is_delete = false;
  has_condition_column_name_ = false;
}

void postgresql_dialect_compiler::visit(const matador::detail::remove &)
{
  is_update = false;
  is_delete = true;
  has_condition_column_name_ = false;
}

void postgresql_dialect_compiler::visit(const matador::detail::tablename &tab)
{
  tablename_ = tab.table_name;
}

void postgresql_dialect_compiler::visit(const matador::detail::from &tab)
{
  tablename_ = tab.table_name;
}

void postgresql_dialect_compiler::visit(const matador::detail::where &whr)
{
  if (is_update || is_delete) {
    where_ = top().current;
    whr.cond->accept(*this);
  }
}

void postgresql_dialect_compiler::visit(const matador::detail::basic_column_condition &cond)
{
  if (!has_condition_column_name_) {
    condition_column_name_ = cond.field_.name;
    has_condition_column_name_ = true;
  }
}

void postgresql_dialect_compiler::visit(const matador::detail::top &limit)
{
  if (!is_update && !is_delete) {
    return;
  }

  if (!has_condition_column_name_) {
    return;
  }

  column rowid(condition_column_name_);
  auto where_token = std::static_pointer_cast<detail::where>(*where_);
  auto sub_select = matador::select({rowid}).from(tablename_).where(where_token->cond).limit(limit.limit_);
  auto cond = make_condition(equals(rowid, sub_select));

  where_token->cond.swap(cond);

  top().tokens_.erase(top().current);
}

void postgresql_dialect_compiler::on_compile_start()
{
  basic_dialect_compiler::on_compile_start();
}

}
}