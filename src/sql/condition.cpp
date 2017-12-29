#include "matador/sql/condition.hpp"

namespace matador {

namespace detail {

std::array<std::string, basic_condition::num_operands>
  basic_condition::operands = { {"=", "<>", "<", "<=", ">", ">=", "OR", "AND", "NOT"} };

}

condition<column, detail::basic_query> in(const matador::column &f, detail::basic_query &q)
{
  return condition<column, detail::basic_query>(f, q);
}

}
