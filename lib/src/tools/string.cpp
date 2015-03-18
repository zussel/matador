//
// Created by sascha on 3/18/15.
//

#include "tools/string.hpp"
#include "tools/time.hpp"

#include <stdexcept>

namespace oos {

std::string to_string(const oos::time &x, const char *format)
{
}

std::string to_string(const oos::date &x, const char *format)
{
  struct tm timeinfo;
  timeinfo.tm_mon = x.month() - 1;
  timeinfo.tm_year = x.year() - 1900;
  timeinfo.tm_mday = x.day();

  char buffer[80];
  if (strftime(buffer, 80, format, &timeinfo) == 0) {
    throw std::logic_error("couldn't format date string");
  }
  return buffer;
}

}
