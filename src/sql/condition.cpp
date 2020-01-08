#include "matador/sql/condition.hpp"

namespace matador {

namespace detail {

std::array<std::string, basic_condition::num_operands>
  basic_condition::operands = { {"=", "<>", "<", "<=", ">", ">=", "OR", "AND", "NOT", "IN", "LIKE"} };

}

condition<column, detail::basic_query> equals(const column &col, detail::basic_query &q)
{
  return condition<column, detail::basic_query>(col, detail::basic_condition::EQUAL, q);
}

condition<column, detail::basic_query> in(const column &f, detail::basic_query &q)
{
  return condition<column, detail::basic_query>(f, detail::basic_condition::IN_LIST, q);
}

condition<column, std::string> like(const matador::column &col, const std::string &val)
{
  return condition<column, std::string>(col, detail::basic_condition::LIKE, val);
}

}
