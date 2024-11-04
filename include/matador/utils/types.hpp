#ifndef MATADOR_TYPES_HPP
#define MATADOR_TYPES_HPP

#include "matador/utils/date.hpp"
#include "matador/utils/placeholder.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/data_types.hpp"

#include <variant>
#include <vector>
#include <string>

namespace matador::utils {

using byte = unsigned char;
using blob = std::vector<byte>;

using database_type = std::variant<
    uint8_t, uint16_t, uint32_t, uint64_t,
    int8_t, int16_t, int32_t, int64_t,
    float, double,
    bool,
    std::string,
    time, date,
    blob,
    placeholder,
    nullptr_t>;

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

void initialize_by_data_type(data_type type, any_type &val);

}

#endif //MATADOR_TYPES_HPP
