//
// Created by sascha on 3/18/15.
//

#include "tools/string.hpp"
#include "tools/time.hpp"

#include <stdexcept>
#include <cstring>

namespace oos {

std::string to_string(const oos::time &x, const char *format)
{
  struct tm timeinfo = x.get_tm();

  char buffer[255];
  if (strftime(buffer, 255, format, &timeinfo) == 0) {
    throw std::logic_error("couldn't format date string");
  }
  std::string result(buffer);
  // check for %f
  auto pos = result.find("%f");
  if (pos != std::string::npos) {
    std::string millis = std::to_string(x.milli_second());
    // replace %f with millis
    result.replace(pos, 2, millis);
  }
  return result;
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
