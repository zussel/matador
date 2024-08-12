#ifndef MATADOR_TYPES_HPP
#define MATADOR_TYPES_HPP

#include "matador/utils/date.hpp"
#include "matador/utils/placeholder.hpp"
#include "matador/utils/time.hpp"

#include <variant>
#include <vector>
#include <string>

namespace matador::utils {

using byte = unsigned char;
using blob = std::vector<byte>;

using any_type = std::variant<
  char, short, int, long, long long,
  unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,
  float, double,
  bool,
  const char*,
  std::string,
  date,
  time,
  blob,
  placeholder,
  nullptr_t
>;

struct null_type_t {};

}

#endif //MATADOR_TYPES_HPP
