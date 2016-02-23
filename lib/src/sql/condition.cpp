#include "sql/condition.hpp"

namespace oos {

std::array<std::string, basic_condition::num_operands>
basic_condition::operands = { "=", "<>", "<", "<=", ">", ">=", "OR", "AND", "NOT" };

}
