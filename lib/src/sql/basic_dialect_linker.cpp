//
// Created by sascha on 17.08.16.
//

#include "sql/basic_dialect_linker.hpp"
#include "sql/dialect_token.hpp"

#include <algorithm>

namespace oos {

namespace detail {

void basic_dialect_linker::link(const token_list_t &tokens, detail::build_info *buildinfo)
{
  build_info_stack_.push(buildinfo);
  // build the query
  for(auto tokptr : tokens) {
    tokptr->accept(*this);
  }
  build_info_stack_.pop();
}

void basic_dialect_linker::visit(const oos::detail::begin &begin)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(begin.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::commit &commit)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(commit.type) + " ");}

void basic_dialect_linker::visit(const oos::detail::rollback &rollback)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(rollback.type) + " ");
}

void basic_dialect_linker::visit(oos::detail::query &q)
{
  build_info_stack_.top()->dialect->append_to_result("(");
  link(q.sql_.token_list_, build_info_stack_.top());
//  this->build(q.sql_, compile_type_, false);
  build_info_stack_.top()->dialect->append_to_result(") ");
}

void basic_dialect_linker::visit(const oos::detail::create &create)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(create.type) + " " + create.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::drop &drop)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(drop.type) + " " + drop.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::select &select)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(select.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::distinct &distinct)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(distinct.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::update &update)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(update.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::tablename &table)
{
  build_info_stack_.top()->dialect->append_to_result(table.tab + " ");
}

void basic_dialect_linker::visit(const oos::detail::set &set)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(set.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::as &alias)
{
  std::stringstream res;
  res << token_string(alias.type) << " " << alias.alias << " ";
  build_info_stack_.top()->dialect->append_to_result(res.str());
}

void basic_dialect_linker::visit(const oos::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " " << top.limit_ << " ";
  build_info_stack_.top()->dialect->append_to_result(res.str());
}

void basic_dialect_linker::visit(const oos::detail::remove &del)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(del.type) + " " + del.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::values &values)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(values.type) + " (");

  if (values.values_.size() > 1) {
    std::for_each(values.values_.begin(), values.values_.end() - 1, [&](const std::shared_ptr<detail::basic_value> &val) {
      val->accept(*this);
      build_info_stack_.top()->dialect->append_to_result(", ");
    });
  }
  if (!values.values_.empty()) {
    values.values_.back()->accept(*this);
  }
  build_info_stack_.top()->dialect->append_to_result(") ");
}

void basic_dialect_linker::visit(const oos::detail::basic_value &val)
{
  if (build_info_stack_.top()->dialect->compile_type() == basic_dialect::DIRECT) {
    build_info_stack_.top()->dialect->append_to_result(val.str());
  } else {
    build_info_stack_.top()->dialect->inc_bind_count();
    build_info_stack_.top()->dialect->append_to_result("?");
  }
}

void basic_dialect_linker::visit(const oos::detail::order_by &by)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(by.type) + " " + by.column + " ");
}

void basic_dialect_linker::visit(const oos::detail::asc &asc)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(asc.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::desc &desc)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(desc.type) + " ");
}

void basic_dialect_linker::visit(const oos::detail::group_by &by)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(by.type) + " " + by.column + " ");
}

void basic_dialect_linker::visit(const oos::detail::insert &insert)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(insert.type) + " " + insert.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::from &from)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(from.type) + " " + from.table + " ");
}

void basic_dialect_linker::visit(const oos::detail::where &where)
{
  build_info_stack_.top()->dialect->append_to_result(token_string(where.type) + " ");
  where.cond->accept(*this);
  build_info_stack_.top()->dialect->append_to_result(" ");
}

void basic_dialect_linker::visit(const oos::detail::basic_condition &cond)
{
  cond.evaluate(build_info_stack_.top()->dialect->compile_type());
}

void basic_dialect_linker::visit(const oos::detail::basic_column_condition &cond)
{
  build_info_stack_.top()->dialect->inc_bind_count();
  build_info_stack_.top()->dialect->append_to_result(cond.evaluate(build_info_stack_.top()->dialect->compile_type()));
}

void basic_dialect_linker::visit(const oos::detail::basic_in_condition &cond)
{
  build_info_stack_.top()->dialect->inc_bind_count(cond.size());
  build_info_stack_.top()->dialect->append_to_result(cond.evaluate(build_info_stack_.top()->dialect->compile_type()));
}

void basic_dialect_linker::visit(const oos::columns &cols)
{
  if (cols.with_brackets_ == columns::WITH_BRACKETS) {
    build_info_stack_.top()->dialect->append_to_result("(");
  }

  if (cols.columns_.size() > 1) {
    std::for_each(cols.columns_.begin(), cols.columns_.end() - 1, [&](const std::shared_ptr<column> &col)
    {
      col->accept(*this);
      build_info_stack_.top()->dialect->append_to_result(", ");
    });
  }
  if (!cols.columns_.empty()) {
    cols.columns_.back()->accept(*this);
  }

  if (cols.with_brackets_ == columns::WITH_BRACKETS) {
    build_info_stack_.top()->dialect->append_to_result(")");
  }

  build_info_stack_.top()->dialect->append_to_result(" ");
}

void basic_dialect_linker::visit(const oos::column &col)
{
  build_info_stack_.top()->dialect->append_to_result(col.name);
  build_info_stack_.top()->dialect->inc_column_count();
}

void basic_dialect_linker::visit(const oos::detail::typed_column &col)
{
  build_info_stack_.top()->dialect->append_to_result(col.name + " " + build_info_stack_.top()->dialect->type_string(col.type));
}

void basic_dialect_linker::visit(const oos::detail::identifier_column &col)
{
  build_info_stack_.top()->dialect->append_to_result(col.name + " " + build_info_stack_.top()->dialect->type_string(col.type) + " NOT NULL PRIMARY KEY");
}

void basic_dialect_linker::visit(const oos::detail::typed_varchar_column &col)
{
  std::stringstream str;
  str << col.name << " " << build_info_stack_.top()->dialect->type_string(col.type) << "(" << col.size << ")";
  build_info_stack_.top()->dialect->append_to_result(str.str());
}

void basic_dialect_linker::visit(const oos::detail::identifier_varchar_column &col)
{
  std::stringstream str;
  str << col.name << " " << build_info_stack_.top()->dialect->type_string(col.type) << "(" << col.size << ") NOT NULL PRIMARY KEY";
  build_info_stack_.top()->dialect->append_to_result(str.str());
}

void basic_dialect_linker::visit(const oos::detail::basic_value_column &col)
{
  build_info_stack_.top()->dialect->append_to_result(col.name + "=");
  col.value_->accept(*this);
}

std::string basic_dialect_linker::token_string(detail::token::t_token tok) const
{
  return build_info_stack_.top()->dialect->token_at(tok);
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