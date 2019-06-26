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
}

void postgresql_dialect_compiler::visit(const matador::detail::update &)
{
  is_update = true;
}

void postgresql_dialect_compiler::visit(const matador::detail::remove &)
{
  is_update = false;
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
  if (is_update) {
    where_ = top().current;
  }
}

void postgresql_dialect_compiler::visit(const matador::detail::top &limit)
{
  if (!is_update) {
    return;
  }

  column rowid("owner_id");
  auto where_token = std::static_pointer_cast<detail::where>(*where_);
  auto sub_select = matador::select({rowid}).from(tablename_).where(where_token->cond).limit(limit.limit_);
  auto cond = make_condition(equals(rowid, sub_select));

//  auto first_cond = extract_first_condition(where_);
  where_token->cond.swap(cond);

  top().tokens_.erase(top().current);
}

void postgresql_dialect_compiler::on_compile_start()
{
  basic_dialect_compiler::on_compile_start();
}

//matador::detail::basic_condition postgresql_dialect_compiler::extract_first_condition(
//std::list<std::shared_ptr<matador::detail::token>, std::allocator<std::shared_ptr<matador::detail::token>>>::iterator iterator)
//{
//  return detail::basic_condition();
//}
}

}