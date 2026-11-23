#include "matador/query/mapping/generators.hpp"

namespace matador::query {
column_value_generator::column_value_generator(bool generate_placeholder)
: generate_placeholder_(generate_placeholder) {}

void column_value_generator::on_revision(const char *id, uint64_t &rev) {
  push_back(id, rev);
}

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