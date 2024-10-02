#include "matador/utils/convert.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

namespace matador::utils {

void convert(std::string &dest, bool source)
{
  dest = source ? "true" : "false";
}

void convert(std::string &dest, const char *source)
{
  dest.assign(source);
}

void convert(std::string &dest, const date &source)
{
  dest = matador::utils::to_string(source);
}

void convert(std::string &dest, const time &source)
{
  dest = matador::utils::to_string(source);
}

void convert(utils::blob &dest, const date &source)
{
  convert(dest, source.julian_date());
}

void convert(utils::blob &dest, const time &source)
{
  convert(dest, source.get_time_info().seconds_since_epoch);
}

void convert(utils::blob &, bool /*source*/)
{
  throw std::logic_error("couldn't convert bool to blob");
}

void convert(utils::blob &, const char * /*source*/)
{
  throw std::logic_error("couldn't convert const char* to blob");
}

void convert(utils::blob &, const std::string &/*source*/)
{
  throw std::logic_error("couldn't convert string to blob");
}

void convert(bool &dest, const std::string &source)
{
  dest = source == "t";
}

void convert(bool &/*dest*/, const char * /*source*/)
{
  throw std::logic_error("couldn't convert const char to bool");
}

void convert(bool &/*dest*/, const utils::blob &/*source*/)
{
  throw std::logic_error("couldn't convert blob to bool");
}

void convert(date &/*dest*/, bool /*source*/)
{
  throw std::logic_error("couldn't convert bool to date");
}

void convert(time &/*dest*/, bool /*source*/)
{
  throw std::logic_error("couldn't convert bool to time");
}

void convert(date &dest, const std::string &source)
{
  dest.set(source.c_str(), date_format::ISO8601);
}

void convert(time &dest, const std::string &source)
{
  dest.set(source.c_str(), "%Y-%m-%d %H:%M:%S.%f");
}

void convert(matador::date &dest, const char *source)
{
  dest.set(source);
}

void convert(matador::time &dest, const char *source)
{
  dest.set(source);
}

void convert(date &dest, const date &source)
{
  dest = source;
}

void convert(time &dest, const date &source)
{
  dest.set(source);
}

void convert(date &dest, const time &source)
{
  dest.set(source.day(), source.month(), source.year());
}

void convert(time &dest, const time &source)
{
  dest = source;
}

long long to_long_long(const char *source)
{
  if (strlen(source) == 0) {
    return{};
  }
  char *end;
  const auto result = strtoll(source, &end, 10);
  if (end == nullptr) {
    // Todo: check error
    throw std::logic_error("couldn't convert value to number");
  }

  return result;
}

unsigned long long to_unsigned_long_long(const char *source)
{
  if (strlen(source) == 0) {
    return{};
  }
  char *end;
  const auto result = strtoull(source, &end, 10);
  if (end == nullptr) {
    // Todo: check error
    throw std::logic_error("couldn't convert value to number");
  }

  return result;
}

long double to_double(const char *source)
{
  if (strlen(source) == 0) {
    return{};
  }
  char *end;
  const auto result = strtold(source, &end);
  if (end == nullptr) {
    // Todo: check error
    throw std::logic_error("couldn't convert value to number");
  }

  return result;
}

void convert(utils::blob &dest, const utils::blob &data)
{
  dest = data;
}

void convert(placeholder &dest, const placeholder &source)
{
  dest = source;
}

void convert(nullptr_t &dest, nullptr_t const &source)
{
  dest = source;
}

void convert(std::string &dest, const std::string &source)
{
  dest.assign(source);
}

void convert(bool &, const date &)
{
  throw std::logic_error("couldn't convert bool value to matador::date");
}

void convert(bool &, const time &)
{
  throw std::logic_error("couldn't convert bool value to matador::time");
}

}