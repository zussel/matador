#include "matador/sql/condition.hpp"

namespace matador::sql {

condition<column, utils::placeholder>::condition(const column &fld, const operand_type op, const utils::placeholder &val)
: basic_column_condition(fld, op), value(val)
{}

std::string condition<column, utils::placeholder>::evaluate(const dialect &d, query_context &query) const
{
  query.bind_vars.emplace_back(field_.name);
  return d.prepare_identifier(field_) + " " + operand + " " + d.next_placeholder(query.bind_vars);
}

condition<column, query_context>::condition(column col, const operand_type op, query_context &q)
: basic_column_condition(std::move(col), op), query_(q)
{}

std::string condition<column, query_context>::evaluate(const dialect &d, query_context &/*query*/) const
{
  std::string result(d.prepare_identifier(field_) + " " + operand + " (");
  result += query_.sql;
  result += ")";
  return result;
}

std::string condition<column, column>::evaluate(const dialect &d, query_context &/*query*/) const {
  return d.prepare_condition(field_) + " " + operand + " " + d.prepare_condition(other_column_);
}

condition<column, query_context> in(const column &col, query_context &&q)
{
  return {col, basic_condition::operand_type::IN_LIST, q};
}

condition<column, std::string> like(const column &col, const std::string &val) {
  return { col, basic_condition::operand_type::LIKE, val };
}

condition<column, column> operator==(const column &a, const column &b)
{
  return {a, basic_condition::operand_type::EQUAL, b};
}
}