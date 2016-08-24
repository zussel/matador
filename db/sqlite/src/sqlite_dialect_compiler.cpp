//
// Created by sascha on 6/20/16.
//

#include "sqlite_dialect_compiler.hpp"

#include "sql/query.hpp"
#include "sqlite_dialect.hpp"

namespace oos {

namespace sqlite {

sqlite_dialect_compiler::sqlite_dialect_compiler(sqlite_dialect &dialect)
  : dialect_(dialect)
{ }

void sqlite_dialect_compiler::visit(const oos::detail::select &)
{
  is_delete = false;
  is_update = false;
}

void sqlite_dialect_compiler::visit(const oos::detail::update &)
{
  is_update = true;
  is_delete = false;
}

void sqlite_dialect_compiler::visit(const oos::detail::remove &r)
{
  is_delete = true;
  is_update = false;
  tablename_ = r.table;
}

void sqlite_dialect_compiler::visit(const oos::detail::tablename &tablename)
{
  tablename_ = tablename.tab;
}

void sqlite_dialect_compiler::visit(const oos::detail::where &)
{
  // store condition
  if (is_update || is_delete) {
    where_ = top().current;
  }
}

void sqlite_dialect_compiler::visit(const oos::detail::top &limit)
{
  // if statement was a limited updated statement
  // replace the where clause with a sub select
  if (!is_update && !is_delete) {
    return;
  }

  column rowid("rowid");
  auto where_token = std::static_pointer_cast<detail::where>(*where_);
  auto subselect = oos::select({rowid}).from(tablename_).where(where_token->cond).limit(limit.limit_);
  auto cond = make_condition(oos::in(rowid, subselect, &dialect_));

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