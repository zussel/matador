#include "matador/utils/convert.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/time.hpp"

#include <cstring>

namespace matador::utils {

void convert(std::string &dest, bool source) {
  dest = source ? "true" : "false";
}

void convert(std::string &dest, const char *source) {
  dest.assign(source);
}

long long to_long_long(const char *source) {
  if (strlen(source) == 0) {
    return {};
  }
  char *end;
  const auto result = strtoll(source, &end, 10);
  if (end == nullptr) {
    // Todo: check error
    throw std::logic_error("couldn't convert value to number");
  }

  return result;
}

unsigned long long to_unsigned_long_long(const char *source) {
  if (strlen(source) == 0) {
    return {};
  }
  char *end;
  const auto result = strtoull(source, &end, 10);
  if (end == nullptr) {
    // Todo: check error
    throw std::logic_error("couldn't convert value to number");
  }

  return result;
}

long double to_double(const char *source) {
  if (strlen(source) == 0) {
    return {};
  }
  char *end;
  const auto result = strtold(source, &end);
  if (end == nullptr) {
    // Todo: check error
    throw std::logic_error("couldn't convert value to number");
  }

  return result;
}

void convert(utils::blob &dest, const utils::blob &data) {
  dest = data;
}

void convert(std::string &dest, const date &source) {
  dest = matador::to_string(source);
}

void convert(std::string &dest, const time &source) {
  dest = matador::to_string(source);
}

}