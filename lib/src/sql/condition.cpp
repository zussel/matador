#include "sql/condition.hpp"

namespace oos {

namespace detail {

std::array<std::string, basic_condition::num_operands>
  basic_condition::operands = {"=", "<>", "<", "<=", ">", ">=", "OR", "AND", "NOT"};

}

condition<column, detail::basic_query> in(const oos::column &f, detail::basic_query &q, basic_dialect *dialect)
{
  return condition<column, detail::basic_query>(f, q, dialect);
}

}
