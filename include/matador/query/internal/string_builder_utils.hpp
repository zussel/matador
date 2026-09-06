#ifndef MATADOR_STRING_BUILDER_UTILS_HPP
#define MATADOR_STRING_BUILDER_UTILS_HPP

#include <string>

namespace matador::query {
class dialect;

void prepare_identifier_string_append(std::string& out, std::string_view col, const dialect &d);
}

#endif  // MATADOR_STRING_BUILDER_UTILS_HPP
