#include "matador/sql/condition.hpp"

namespace matador::sql {

condition<column, utils::placeholder, std::enable_if<true>::type>::condition(const column &fld, basic_condition::operand_t op, const utils::placeholder &val)
: basic_column_condition(fld, op), value(val)
{}

std::string condition<column, utils::placeholder, std::enable_if<true>::type>::evaluate(const dialect &d, query_context &query) const
{
  query.bind_vars.emplace_back(field_.name);
  return d.prepare_identifier(field_) + " " + operand + " " + d.next_placeholder(query.bind_vars);
}

condition<column, query_context>::condition(column col, basic_condition::operand_t op, query_context &q)
: basic_column_condition(std::move(col), op), query_(q)
{}

std::string condition<column, query_context>::evaluate(const dialect &d, query_context &/*query*/) const
{
  std::string result(d.prepare_identifier(field_) + " " + operand + " (");
  result += query_.sql;
  result += ")";
  return result;
}

condition<column, query_context> in(const column &col, query_context &&q)
{
  return {col, basic_condition::operand_t::IN_LIST, q};
}

condition<column, std::string> like(const column &col, const std::string &val) {
  return { col, basic_condition::operand_t::LIKE, val };
}

condition<column, column> operator==(const column &a, const column &b)
{
  return {a, basic_condition::operand_t::EQUAL, b};
}
}