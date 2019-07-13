//
// Created by sascha on 17.08.16.
//

#include "matador/sql/basic_dialect_linker.hpp"
#include "matador/sql/dialect_token.hpp"

#include <algorithm>

namespace matador {

namespace detail {

void basic_dialect_linker::link()
{
  // build the query
  for(auto const &tokptr : top().tokens_) {
    tokptr->accept(*this);
  }
}

void basic_dialect_linker::visit(const matador::detail::begin &begin)
{
  dialect().append_to_result(token_string(begin.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::commit &commit)
{
  dialect().append_to_result(token_string(commit.type) + " ");}

void basic_dialect_linker::visit(const matador::detail::rollback &rollback)
{
  dialect().append_to_result(token_string(rollback.type) + " ");
}

void basic_dialect_linker::visit(matador::detail::query &q)
{
  dialect().append_to_result("(");
  dialect().append_to_result(dialect().continue_build(q.sql_, dialect().compile_type()));
  dialect().append_to_result(") ");
}

void basic_dialect_linker::visit(const matador::detail::create &create)
{
  dialect().append_to_result(token_string(create.type) + " " + dialect_->prepare_identifier(create.table_name) + " ");
}

void basic_dialect_linker::visit(const matador::detail::drop &drop)
{
  dialect().append_to_result(token_string(drop.type) + " " + dialect_->prepare_identifier(drop.table_name) + " ");
}

void basic_dialect_linker::visit(const matador::detail::select &select)
{
  dialect().append_to_result(token_string(select.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::distinct &distinct)
{
  dialect().append_to_result(token_string(distinct.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::update &update)
{
  dialect().append_to_result(token_string(update.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::tablename &table)
{
  dialect().append_to_result(dialect_->prepare_identifier(table.table_name) + " ");
}

void basic_dialect_linker::visit(const matador::detail::set &set)
{
  dialect().append_to_result(token_string(set.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::as &alias)
{
  std::stringstream res;
  res << token_string(alias.type) << " " << alias.alias << " ";
  dialect().append_to_result(res.str());
}

void basic_dialect_linker::visit(const matador::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " " << top.limit_ << " ";
  dialect().append_to_result(res.str());
}

void basic_dialect_linker::visit(const matador::detail::remove &del)
{
  dialect().append_to_result(token_string(del.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::values &values)
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

void basic_dialect_linker::visit(const matador::detail::basic_value &val)
{
  if (dialect().compile_type() == basic_dialect::DIRECT) {
    dialect().append_to_result(val.safe_string(dialect()));
  } else {
    dialect().inc_bind_count();
    dialect().append_to_result(dialect().next_placeholder());
  }
}

void basic_dialect_linker::visit(const matador::detail::order_by &by)
{
  dialect().append_to_result(token_string(by.type) + " " + dialect().prepare_identifier(by.column) + " ");
}

void basic_dialect_linker::visit(const matador::detail::asc &asc)
{
  dialect().append_to_result(token_string(asc.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::desc &desc)
{
  dialect().append_to_result(token_string(desc.type) + " ");
}

void basic_dialect_linker::visit(const matador::detail::group_by &by)
{
  dialect().append_to_result(token_string(by.type) + " " + dialect().prepare_identifier(by.column) + " ");
}

void basic_dialect_linker::visit(const matador::detail::insert &insert)
{
  dialect().append_to_result(token_string(insert.type) + " " + dialect_->prepare_identifier(insert.table_name) + " ");
}

void basic_dialect_linker::visit(const matador::detail::from &from)
{
  if (from.table_name.empty()) {
    dialect().append_to_result(token_string(from.type) + " ");
  } else {
    dialect().append_to_result(token_string(from.type) + " " + dialect_->prepare_identifier(from.table_name) + " ");
  }
}

void basic_dialect_linker::visit(const matador::detail::where &where)
{
  dialect().append_to_result(token_string(where.type) + " ");
  dialect().append_to_result(where.cond->evaluate(dialect()));
//  where.cond->accept(*this);
  dialect().append_to_result(" ");
}

void basic_dialect_linker::visit(const matador::detail::basic_condition &cond)
{
  dialect().append_to_result(cond.evaluate(dialect()));
}

void basic_dialect_linker::visit(const matador::detail::basic_column_condition &cond)
{
  dialect().append_to_result(cond.evaluate(dialect()));
}

void basic_dialect_linker::visit(const matador::detail::basic_in_condition &cond)
{
  dialect().append_to_result(cond.evaluate(dialect()));
}

void basic_dialect_linker::visit(const matador::columns &cols)
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

void basic_dialect_linker::visit(const matador::column &col)
{
  if (col.skip_quotes) {
    dialect().append_to_result(col.name);
  } else {
    dialect().append_to_result(dialect_->prepare_identifier(col.name));
  }
  dialect().inc_column_count();
}

void basic_dialect_linker::visit(const matador::detail::typed_column &col)
{
  visit(static_cast<const matador::column&>(col));
  dialect().append_to_result(std::string(" ") + dialect().type_string(col.type));
}

void basic_dialect_linker::visit(const matador::detail::typed_identifier_column &col)
{
  visit(static_cast<const matador::column&>(col));
  dialect().append_to_result(std::string(" ") + dialect().type_string(col.type) + " NOT NULL PRIMARY KEY");
}

void basic_dialect_linker::visit(const matador::detail::typed_varchar_column &col)
{
  visit(static_cast<const matador::column&>(col));
  std::stringstream str;
  str << " " << dialect().type_string(col.type) << "(" << col.size << ")";
  dialect().append_to_result(str.str());
}

void basic_dialect_linker::visit(const matador::detail::identifier_varchar_column &col)
{
  visit(static_cast<const matador::column&>(col));
  std::stringstream str;
  str << " " << dialect().type_string(col.type) << "(" << col.size << ") NOT NULL PRIMARY KEY";
  dialect().append_to_result(str.str());
}

void basic_dialect_linker::visit(const matador::detail::basic_value_column &col)
{
  visit(static_cast<const matador::column&>(col));
  dialect().append_to_result("=");
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

void basic_dialect_linker::append_to_result(basic_dialect &dialect, const std::string &part)
{
  dialect.append_to_result(part);
}

void basic_dialect_linker::dialect(basic_dialect *d)
{
  dialect_ = d;
}

}

}