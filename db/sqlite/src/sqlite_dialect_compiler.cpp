//
// Created by sascha on 6/20/16.
//

#include <sql/dialect_token.hpp>
#include "sqlite_dialect_compiler.hpp"

#include "sql/column.hpp"

namespace oos {

namespace sqlite {

sqlite_dialect_compiler::sqlite_dialect_compiler() { }

void sqlite_dialect_compiler::visit(const oos::detail::update &update1)
{
  is_update = true;
}

void sqlite_dialect_compiler::visit(const oos::columns &columns)
{
  // collect all columns
  if (is_update) {
    columns_ = columns.columns_;
  }
}

void sqlite_dialect_compiler::visit(const oos::detail::remove &remove1)
{
  is_delete;
}

void sqlite_dialect_compiler::visit(const oos::detail::where &where)
{
  // store condition
  if (is_update) {
    condition_ = where.cond;
  }
}

void sqlite_dialect_compiler::visit(const oos::detail::basic_column_condition &condition)
{
  // collect all column conditions
  auto bccptr = std::static_pointer_cast<detail::basic_column_condition>(*token_data_stack_.top().current_);
  column_conditions_.push_back(bccptr);
}

void sqlite_dialect_compiler::visit(const oos::detail::top &top1)
{
  // if statement was a limited updated statement
  // replace the where clause with a sub select
  if (!is_update || !is_delete) {
    return;
  }

  
}

void sqlite_dialect_compiler::on_compile_start()
{
  columns_.clear();
  column_conditions_.clear();
  condition_.reset();
}

// Todo: find limit for update/delete and replace it with

/*
 * update <table> set item_id=8 where owner_id=? and item_id=? limit 1
 * =>
 * update <table> set item_id=8 where
 *   owner_id in (
 *     select * from (
 *       select owner_id from owner_item where owner_id=? and item_id=? limit 1
 *     )
 *     as p
 *   )
 *
 * delete from <table> where owner_id=? and item_id=? limit 1
 *                           ------------------------
 * =>
 * delete from <table> where
 *   owner_id in (
 *     select * from (
 *       select owner_id from owner_item where owner_id=? and item_id=? limit 1
 *                                             ------------------------
 *     )
 *     as p
 *   )
 */

}
}