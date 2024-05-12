#ifndef MATADOR_COLUMN_VALUE_HPP
#define MATADOR_COLUMN_VALUE_HPP

#include "matador/sql/placeholder.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/types.hpp"

#include <string>
#include <variant>

namespace matador::sql {

using column_type = std::variant<
  char, short, int, long, long long,
  unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,
  float, double,
  bool,
  const char*,
  std::string,
  date,
  time,
  utils::blob,
  placeholder,
  nullptr_t>;

}
#endif //MATADOR_COLUMN_VALUE_HPP
