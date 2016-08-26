//
// Created by sascha on 17.08.16.
//

#include "sql/basic_dialect_linker.hpp"
#include "sql/dialect_token.hpp"

#include <algorithm>

namespace oos {

namespace detail {

void basic_dialect_linker::link()
{
  // build the query
  for(auto tokptr : top().tokens_) {
    tokptr->accept(*this);
  }
}

void basic_dialect_linker::visit(const oos::detail::begin &begin)
{
  dialect().append_to_result(token_string(begin.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::commit &commit)
{
  dialect().append_to_result(token_string(commit.type) + " ");}

void basic_dialect_linker::visit(const oos::detail::rollback &rollback)
{
  dialect().append_to_result(token_string(rollback.type) + " ");
}

void basic_dialect_linker::visit(oos::detail::query &q)
{
  dialect().append_to_result("(");
  dialect().append_to_result(dialect().build(q.sql_, dialect().compile_type()));
  dialect().append_to_result(") ");
}

void basic_dialect_linker::visit(const oos::detail::create &create)
{
  dialect().append_to_result(token_string(create.type) + " " + create.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::drop &drop)
{
  dialect().append_to_result(token_string(drop.type) + " " + drop.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::select &select)
{
  dialect().append_to_result(token_string(select.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::distinct &distinct)
{
  dialect().append_to_result(token_string(distinct.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::update &update)
{
  dialect().append_to_result(token_string(update.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::tablename &table)
{
  dialect().append_to_result(table.tab + " ");
}

void basic_dialect_linker::visit(const oos::detail::set &set)
{
  dialect().append_to_result(token_string(set.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::as &alias)
{
  std::stringstream res;
  res << token_string(alias.type) << " " << alias.alias << " ";
  dialect().append_to_result(res.str());
}

void basic_dialect_linker::visit(const oos::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " " << top.limit_ << " ";
  dialect().append_to_result(res.str());
}

void basic_dialect_linker::visit(const oos::detail::remove &del)
{
  dialect().append_to_result(token_string(del.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::values &values)
{
  dialect().append_to_result(token_string(values.type) + " (");

  if (values.values_.size() > 1) {
    std::for_each(values.values_.begin(), values.values_.end() - 1, [&](const std::shared_ptr<detail::basic_value> &val) {
      val->accept(*this);
      dialect().append_to_result(", ");
    });
  }
  if (!values.values_.empty()) {
    values.values_.back()->accept(*this);
  }
  dialect().append_to_result(") ");
}

void basic_dialect_linker::visit(const oos::detail::basic_value &val)
{
  if (dialect().compile_type() == basic_dialect::DIRECT) {
    dialect().append_to_result(val.str());
  } else {
    dialect().inc_bind_count();
    dialect().append_to_result("?");
  }
}

void basic_dialect_linker::visit(const oos::detail::order_by &by)
{
  dialect().append_to_result(token_string(by.type) + " " + by.column + " ");
}

void basic_dialect_linker::visit(const oos::detail::asc &asc)
{
  dialect().append_to_result(token_string(asc.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::desc &desc)
{
  dialect().append_to_result(token_string(desc.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::group_by &by)
{
  dialect().append_to_result(token_string(by.type) + " " + by.column + " ");
}

void basic_dialect_linker::visit(const oos::detail::insert &insert)
{
  dialect().append_to_result(token_string(insert.type) + " " + insert.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::from &from)
{
  dialect().append_to_result(token_string(from.type) + " " + from.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::where &where)
{
  dialect().append_to_result(token_string(where.type) + " ");
  where.cond->accept(*this);
  dialect().append_to_result(" ");
}

void basic_dialect_linker::visit(const oos::detail::basic_condition &cond)
{
  dialect().append_to_result(cond.evaluate(dialect().compile_type()));
//  cond.evaluate(dialect().compile_type());
}

void basic_dialect_linker::visit(const oos::detail::basic_column_condition &cond)
{
  dialect().inc_bind_count();
  dialect().append_to_result(cond.evaluate(dialect().compile_type()));
}

void basic_dialect_linker::visit(const oos::detail::basic_in_condition &cond)
{
  dialect().inc_bind_count(cond.size());
  dialect().append_to_result(cond.evaluate(dialect().compile_type()));
}

void basic_dialect_linker::visit(const oos::columns &cols)
{
  if (cols.with_brackets_ == columns::WITH_BRACKETS) {
    dialect().append_to_result("(");
  }

  if (cols.columns_.size() > 1) {
    std::for_each(cols.columns_.begin(), cols.columns_.end() - 1, [&](const std::shared_ptr<column> &col)
    {
      col->accept(*this);
      dialect().append_to_result(", ");
    });
  }
  if (!cols.columns_.empty()) {
    cols.columns_.back()->accept(*this);
  }

  if (cols.with_brackets_ == columns::WITH_BRACKETS) {
    dialect().append_to_result(")");
  }

  dialect().append_to_result(" ");
}

void basic_dialect_linker::visit(const oos::column &col)
{
  dialect().append_to_result(col.name);
  dialect().inc_column_count();
}

void basic_dialect_linker::visit(const oos::detail::typed_column &col)
{
  dialect().append_to_result(col.name + " " + dialect().type_string(col.type));
}

void basic_dialect_linker::visit(const oos::detail::identifier_column &col)
{
  dialect().append_to_result(col.name + " " + dialect().type_string(col.type) + " NOT NULL PRIMARY KEY");
}

void basic_dialect_linker::visit(const oos::detail::typed_varchar_column &col)
{
  std::stringstream str;
  str << col.name << " " << dialect().type_string(col.type) << "(" << col.size << ")";
  dialect().append_to_result(str.str());
}

void basic_dialect_linker::visit(const oos::detail::identifier_varchar_column &col)
{
  std::stringstream str;
  str << col.name << " " << dialect().type_string(col.type) << "(" << col.size << ") NOT NULL PRIMARY KEY";
  dialect().append_to_result(str.str());
}

void basic_dialect_linker::visit(const oos::detail::basic_value_column &col)
{
  dialect().append_to_result(col.name + "=");
  col.value_->accept(*this);
}

std::string basic_dialect_linker::token_string(detail::token::t_token tok) const
{
  return dialect().token_at(tok);
}

basic_dialect &basic_dialect_linker::dialect() const
{
  return *dialect_;
}

build_info &basic_dialect_linker::top() const
{
  return dialect_->top();
}

void basic_dialect_linker::dialect(basic_dialect *d)
{
  dialect_ = d;
}

}

}