#include "matador/query/internal/string_builder_utils.hpp"

#include "matador/query/dialect.hpp"

namespace matador::query {
void prepare_identifier_string_append(std::string& out, const std::string_view col, const dialect &d) {
  bool first_part = true;

  const char sq = d.start_quote()[0];
  const char eq = d.end_quote()[0];

  std::size_t i = 0;
  while (true) {
    const std::size_t start = i;

    // find end of part
    while (i < col.size() && col[i] != '.') {
      ++i;
    }
    const std::size_t end = i; // [start, end) is the part

    if (!first_part) {
      out.push_back('.');
    }
    first_part = false;

    // quote_identifier(part) + escape_quotes_in_identifier(part), but streaming:
    out.push_back(sq);
    for (std::size_t j = start; j < end; ++j) {
      const char c = col[j];
      if (c == eq) out.push_back(eq); // escape " as ""
      out.push_back(c);
    }
    out.push_back(eq);

    if (i >= col.size()) break; // done
    ++i; // skip '.'
  }
}
}