#include "matador/sql/basic_condition.hpp"

namespace matador::sql {

std::unordered_map<basic_condition::operand_type, std::string> basic_condition::operands{
  {operand_type::EQUAL, "="},
  {operand_type::NOT_EQUAL, "<>"},
  {operand_type::LESS, "<"},
  {operand_type::LESS_EQUAL, "<="},
  {operand_type::GREATER, ">"},
  {operand_type::GREATER_EQUAL, ">="},
  {operand_type::OR, "OR"},
  {operand_type::AND, "AND"},
  {operand_type::NOT, "NOT"},
  {operand_type::IN_LIST, "IN"},
  {operand_type::LIKE, "LIKE"}
};

basic_column_condition::basic_column_condition(column fld, const operand_type op)
  : field_(std::move(fld)), operand(operands[op])
{ }

basic_in_condition::basic_in_condition(column fld)
  : field_(std::move(fld))
{ }
}