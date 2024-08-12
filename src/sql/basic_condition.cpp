#include "matador/sql/basic_condition.hpp"

namespace matador::sql {

std::unordered_map<basic_condition::operand_t, std::string> basic_condition::operands{
  {operand_t::EQUAL, "="},
  {operand_t::NOT_EQUAL, "<>"},
  {operand_t::LESS, "<"},
  {operand_t::LESS_EQUAL, "<="},
  {operand_t::GREATER, ">"},
  {operand_t::GREATER_EQUAL, ">="},
  {operand_t::OR, "OR"},
  {operand_t::AND, "AND"},
  {operand_t::NOT, "NOT"},
  {operand_t::IN_LIST, "IN"},
  {operand_t::LIKE, "LIKE"}
};

basic_column_condition::basic_column_condition(column fld, basic_condition::operand_t op)
  : field_(std::move(fld)), operand(basic_condition::operands[op])
{ }

basic_in_condition::basic_in_condition(column fld)
  : field_(std::move(fld))
{ }
}