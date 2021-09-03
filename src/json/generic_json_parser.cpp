//
// Created by sascha on 21.01.20.
//

#include "matador/json/generic_json_parser.hpp"

namespace matador {
bool is_error(const char *start, const char *end, long long value) {
  return start == end || (errno == ERANGE && (value == 0 || value == LLONG_MAX || value == LLONG_MIN));
}

bool is_error(const char *start, const char *end, double value) {
  return start == end || (errno == ERANGE && (value == 0 || value == -HUGE_VAL || value == HUGE_VAL));
}
}