//
// Created by sascha on 6/20/16.
//

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
  tablename_ = tab.tab;
}

void sqlite_dialect_compiler::visit(const matador::detail::from &from1)
{
  tablename_ = from1.table;
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
  auto where_token = std::static_pointer_cast<detail::where>(*where_);
  auto subselect = matador::select({rowid}).from(tablename_).where(where_token->cond).limit(limit.limit_);
  auto cond = make_condition(matador::in(rowid, subselect));

  where_token->cond.swap(cond);

  top().tokens_.erase(top().current);
}

void sqlite_dialect_compiler::on_compile_start()
{
  where_ = top().tokens_.end();
  tablename_.clear();
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