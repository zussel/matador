#include "sqlite_query_compiler.hpp"

#include "matador/sql/condition.hpp"
#include "matador/sql/query_compile_context.hpp"

namespace matador::backends::sqlite {

void sqlite_query_compiler::visit(sql::query_select_part &select_part) {
  is_delete = false;
  is_update = false;
  query_compiler::visit(select_part);
}

void sqlite_query_compiler::visit(sql::query_update_part &update_part) {
  is_update = true;
  is_delete = false;
  query_compiler::visit(update_part);
}

void sqlite_query_compiler::visit(sql::query_delete_part &delete_part) {
  is_update = false;
  is_delete = true;
  query_compiler::visit(delete_part);
}

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
void sqlite_query_compiler::visit(sql::query_where_part &where_part)
{
  using namespace matador::sql;
  if (is_delete || is_update) {
//    query_context ctx;
//    ctx.sql = where_part.condition().evaluate(data_->dialect, ctx);
//    sql::query_where_part sqlite_where_part(sql::in("rowid"_col, std::move(ctx)));
//    query_compiler::visit(sqlite_where_part);
    query_compiler::visit(where_part);
  } else {
    query_compiler::visit(where_part);
  }
}

void sqlite_query_compiler::visit(sql::query_limit_part &limit_part) {
  if (is_delete || is_update) {
    query_compiler::visit(limit_part);
  } else {
    query_compiler::visit(limit_part);
  }
}

}
