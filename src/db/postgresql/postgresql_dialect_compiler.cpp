//
// Created by sascha on 24.06.19.
//
#include "matador/db/postgresql/postgresql_dialect_compiler.hpp"

namespace matador {
namespace postgresql {

void postgresql_dialect_compiler::visit(const matador::detail::update &)
{
  is_update = true;
}

void postgresql_dialect_compiler::visit(const matador::detail::tablename &tab)
{
  tablename_ = tab.table_name;
}

void postgresql_dialect_compiler::visit(const matador::detail::from &tab)
{
  tablename_ = tab.table_name;
}

void postgresql_dialect_compiler::visit(const matador::detail::where &)
{
  if (is_update) {
    where_ = top().current;
  }
}

void postgresql_dialect_compiler::visit(const matador::detail::top &)
{
  if (!is_update) {
    return;
  }

//  column rowid("rowid");
//  auto where_token = std::static_pointer_cast<detail::where>(*where_);
//  auto sub_select = matador::select({rowid}).from(tablename_).where(where_token->cond).limit(limit.limit_);
//  auto cond = make_condition(matador::in(rowid, sub_select));
//
//  where_token->cond.swap(cond);
//
//  top().tokens_.erase(top().current);
}

void postgresql_dialect_compiler::on_compile_start()
{
  basic_dialect_compiler::on_compile_start();
}
}

}