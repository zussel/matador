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
}

void postgresql_dialect_compiler::visit(const matador::detail::update &)
{
  is_update = true;
  is_delete = false;
}

void postgresql_dialect_compiler::visit(const matador::detail::remove &)
{
  is_update = false;
  is_delete = true;
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

void postgresql_dialect_compiler::visit(const matador::detail::top &limit)
{
  if (!is_update && !is_delete) {
    return;
  }

  column rowid("ctid");
  auto where_token = std::static_pointer_cast<detail::where>(*where_);
  auto sub_select = matador::select({rowid}).from(tablename_).where(where_token->cond).limit(limit.limit_);
  auto cond = make_condition(equals(rowid, sub_select));

  where_token->cond.swap(cond);

  top().tokens_.erase(top().current);
}

void postgresql_dialect_compiler::on_compile_start()
{
  where_ = top().tokens_.end();
  tablename_.clear();
}

}
}