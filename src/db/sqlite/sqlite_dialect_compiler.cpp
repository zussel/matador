#include "matador/db/sqlite/sqlite_dialect_compiler.hpp"

#include "matador/sql/query.hpp"
#include "matador/db/sqlite/sqlite_dialect.hpp"

namespace matador {

namespace sqlite {

void sqlite_dialect_compiler::visit(const matador::detail::select &)
{
  is_delete = false;
  is_update = false;
}

void sqlite_dialect_compiler::visit(const matador::detail::update &)
{
  is_update = true;
  is_delete = false;
}

void sqlite_dialect_compiler::visit(const matador::detail::remove &)
{
  is_delete = true;
  is_update = false;
}

void sqlite_dialect_compiler::visit(const matador::detail::tablename &tab)
{
  table_name_ = tab.table_name;
}

void sqlite_dialect_compiler::visit(const matador::detail::from &from1)
{
  table_name_ = from1.table_name;
}

void sqlite_dialect_compiler::visit(const matador::detail::where &)
{
  // store condition
  if (is_update || is_delete) {
    where_ = top().current;
  }
}

void sqlite_dialect_compiler::visit(const matador::detail::top &limit)
{
  // if statement was a limited updated statement
  // replace the where clause with a sub select
  if (!is_update && !is_delete) {
    return;
  }

  column rowid("rowid");
  auto *where_token = static_cast<detail::where*>(where_->get()); // NOLINT(*-pro-type-static-cast-downcast)
  auto sub_select = matador::select({rowid}).from(table_name_).where(where_token->cond).limit(limit.limit_);
  auto cond = make_condition(matador::in(rowid, sub_select));

  where_token->cond.swap(cond);

  top().tokens_.erase(top().current);
}

void sqlite_dialect_compiler::on_compile_start()
{
  where_ = top().tokens_.end();
  table_name_.clear();
}

// Todo: find limit for update/delete and replace it with

/*
 * update <table> set <columns> where <cond> limit 1
 * =>
 * UPDATE <table> set <column> WHERE
 *   rowid in (
 *    select rowid FROM <table> WHERE <cond> LIMIT 1);
 *
 *                           ------------------------
 * delete from <table> where <cond> limit 1
 * =>
 * delete from <table> WHERE
 *   rowid in (
 *    select rowid FROM <table> WHERE <cond> LIMIT 1);
 */

}
}