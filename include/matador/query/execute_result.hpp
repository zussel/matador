#ifndef MATADOR_EXECUTE_RESULT_HPP
#define MATADOR_EXECUTE_RESULT_HPP

#include "matador/query/identifier.hpp"

#include <vector>

namespace matador::query {
struct execute_result {
  size_t affected_rows{};
  std::vector<identifier> generated_ids{};
};
}
#endif  // MATADOR_EXECUTE_RESULT_HPP
