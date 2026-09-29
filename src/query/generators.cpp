#include "matador/query/generators.hpp"

namespace matador::query {
void placeholder_generator::on_revision(const char * /*id*/, uint64_t &) {
  result_.emplace_back(_);
}

std::vector<placeholder> placeholders(const size_t num) {
  std::vector<placeholder> result;
  for (size_t i = 0; i < num; ++i) {
    result.emplace_back(_);
  }

  return result;
}
} // namespace matador::query