//
// Created by sascha on 3/4/16.
//
#include "sql/basic_dialect.hpp"
#include "sql/sql.hpp"
#include "sql/dialect_token.hpp"
#include <algorithm>

namespace oos {

std::string basic_dialect::direct(const sql &s)
{
  compile_type_ = DIRECT;
  return s.compile(*this);
}

std::string basic_dialect::prepare(const sql &s)
{
  compile_type_ = PREPARED;
  return s.compile(*this);
}

void basic_dialect::build(const sql &s)
{

}

bool basic_dialect::is_preparing() const
{
  return compile_type_ == PREPARED;
}

std::string basic_dialect::result() const
{
  return result_;
}

void basic_dialect::visit(const oos::detail::create &create)
{
  append_to_result(token_string(create.type) + " " + create.table + " ");
}

void basic_dialect::visit(const oos::detail::drop &drop)
{
  append_to_result(token_string(drop.type) + " " + drop.table + " ");
}

void basic_dialect::visit(const oos::detail::select &select)
{
  append_to_result(token_string(select.type) + " ");
}

void basic_dialect::visit(const oos::detail::distinct &distinct)
{
  append_to_result(token_string(distinct.type) + " ");
}

void basic_dialect::visit(const oos::detail::update &update)
{
  append_to_result(token_string(update.type) + " " + update.table + " ");
}

void basic_dialect::visit(const oos::detail::set &set)
{
  append_to_result(token_string(set.type) + " ");
}

void basic_dialect::visit(const oos::detail::as &alias)
{
  std::stringstream res;
  res << token_string(alias.type) << " " << alias.alias << " ";
  append_to_result(res.str());
}

void basic_dialect::visit(const oos::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " " << top.limit_ << " ";
  append_to_result(res.str());
}

void basic_dialect::visit(const oos::detail::remove &del)
{
  append_to_result(token_string(del.type) + " " + del.table + " ");
}

void basic_dialect::visit(const oos::detail::values &values)
{
  append_to_result(token_string(values.type) + " (");

  if (values.values_.size() > 1) {
    std::for_each(values.values_.begin(), values.values_.end() - 1, [&](const std::shared_ptr<detail::basic_value> &val) {
      val->accept(*this);
      append_to_result(", ");
    });
  }
  if (!values.values_.empty()) {
    values.values_.back()->accept(*this);
  }
  append_to_result(") ");
}

void basic_dialect::visit(const oos::detail::basic_value &val)
{
  if (compile_type() == DIRECT) {
    append_to_result(val.str());
  } else {
    append_to_result("?");
  }
}

void basic_dialect::visit(const oos::detail::order_by &by)
{
  append_to_result(token_string(by.type) + " " + by.column + " ");
}

void basic_dialect::visit(const oos::detail::asc &asc)
{
  append_to_result(token_string(asc.type) + " ");
}

void basic_dialect::visit(const oos::detail::desc &desc)
{
  append_to_result(token_string(desc.type) + " ");
}

void basic_dialect::visit(const oos::detail::group_by &by)
{
  append_to_result(token_string(by.type) + " " + by.column + " ");
}

void basic_dialect::visit(const oos::detail::insert &insert)
{
  append_to_result(token_string(insert.type) + " " + insert.table + " ");
}

void basic_dialect::visit(const oos::detail::from &from)
{
  append_to_result(token_string(from.type) + " " + from.table + " ");
}

void basic_dialect::visit(const oos::detail::where &where)
{
  append_to_result(token_string(where.type) + " ");
  where.cond->accept(*this);
  append_to_result(" ");
}

void basic_dialect::visit(const oos::detail::basic_condition &cond)
{
  append_to_result(cond.compile(*this));
}

void basic_dialect::visit(const oos::columns &cols)
{
  if (cols.with_brackets_ == columns::WITH_BRACKETS) {
    append_to_result("(");
  }

  if (cols.columns_.size() > 1) {
    std::for_each(cols.columns_.begin(), cols.columns_.end() - 1, [&](const std::shared_ptr<column> &col)
    {
      col->accept(*this);
      append_to_result(", ");
    });
  }
  if (!cols.columns_.empty()) {
    cols.columns_.back()->accept(*this);
  }

  if (cols.with_brackets_ == columns::WITH_BRACKETS) {
    append_to_result(")");
  }

  append_to_result(" ");
}

void basic_dialect::visit(const oos::column &col)
{
  append_to_result(col.name);
}

void basic_dialect::visit(const oos::detail::typed_column &col)
{
  append_to_result(col.name + " " + type_string(col.type));
}

void basic_dialect::visit(const oos::detail::identifier_column &col)
{
  append_to_result(col.name + " " + type_string(col.type) + " NOT NULL PRIMARY KEY");
}

void basic_dialect::visit(const oos::detail::typed_varchar_column &col)
{
  std::stringstream str;
  str << col.name << " " << type_string(col.type) << "(" << col.size << ")";
  append_to_result(str.str());
}

void basic_dialect::visit(const oos::detail::identifier_varchar_column &col)
{
  std::stringstream str;
  str << col.name << " " << type_string(col.type) << "(" << col.size << ") NOT NULL PRIMARY KEY";
  append_to_result(str.str());
}

void basic_dialect::visit(const oos::detail::basic_value_column &)
{

}

void basic_dialect::replace_token(detail::token::t_token tkn, const std::string &value)
{
  tokens[tkn] = value;
}

void basic_dialect::visit(const oos::detail::begin &begin)
{
  append_to_result(token_string(begin.type) + " ");
}

void basic_dialect::visit(const oos::detail::commit &commit)
{
  append_to_result(token_string(commit.type) + " ");}

void basic_dialect::visit(const oos::detail::rollback &rollback)
{
  append_to_result(token_string(rollback.type) + " ");
}

void basic_dialect::visit(const oos::detail::query &q)
{
  this->build(q.sql_);
}

void basic_dialect::append_to_result(const std::string &part)
{
  result_ += part;
}

}