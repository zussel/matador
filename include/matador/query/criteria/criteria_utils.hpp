#ifndef MATADOR_CRITERIA_UTILS_HPP
#define MATADOR_CRITERIA_UTILS_HPP

#include "matador/query/placeholder.hpp"
#include "matador/query/column_value.hpp"

namespace matador::query {
using criteria_value = std::variant<placeholder, column_value>;
}

#endif //MATADOR_CRITERIA_UTILS_HPP