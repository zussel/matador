//
// Created by sascha on 6/20/16.
//

#include "sqlite_dialect_compiler.hpp"

#include "sql/dialect_token.hpp"
#include "sql/query.hpp"
#include "sql/column.hpp"

namespace oos {

namespace sqlite {

sqlite_dialect_compiler::sqlite_dialect_compiler() { }

void sqlite_dialect_compiler::visit(const oos::detail::update &update1)
{
  is_update = true;
}

void sqlite_dialect_compiler::visit(const oos::detail::remove &remove1)
{
  is_delete = true;
}

void sqlite_dialect_compiler::visit(const oos::detail::tablename &tablename)
{
  tablename_ = tablename.tab;
}

void sqlite_dialect_compiler::visit(const oos::detail::where &where)
{
  // store condition
  if (is_update || is_delete) {
    where_ = token_data_stack_.top().current_;
  }
}

void sqlite_dialect_compiler::visit(const oos::detail::top &top)
{
  // if statement was a limited updated statement
  // replace the where clause with a sub select
  if (!is_update && !is_delete) {
    return;
  }

  column rowid("rowid");
  auto where_token = std::static_pointer_cast<detail::where>(*where_);
  auto subselect = oos::select({rowid}).from(tablename_).where(rowid == 1).limit(top.limit_);
//  auto subselect = oos::select({rowid}).from(tablename_).where(where_token->cond).limit(top.limit_);

  where_token->cond.reset();
  where_token->cond = make_condition(oos::in(rowid, {7,5,5,8}));
//  where_token->cond = make_condition(oos::in(rowid, subselect));

  token_data_stack_.top().tokens_.erase(token_data_stack_.top().current_);

}

void sqlite_dialect_compiler::on_compile_start()
{
  where_ = token_data_stack_.top().tokens_.end();
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