//
// Created by sascha on 3/18/15.
//

#include "matador/utils/string.hpp"
#include "matador/utils/time.hpp"

#include <stdexcept>
#include <cstring>

#include <ctime>

namespace matador {

size_t split(const std::string &str, char delim, std::vector<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}

size_t split(const std::string &str, char delim, std::list<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}

#ifdef _MSC_VER
const char* date_format::ISO8601 = "%Y-%m-%d";
const char* time_format::ISO8601 = "%Y-%m-%dT%H:%M:%S";
#endif

std::string trim(const std::string& str, const std::string& whitespace)
{
  const auto first = str.find_first_not_of(whitespace);
  if (first == std::string::npos)
    return ""; // no content

  const auto end = str.find_last_not_of(whitespace);
  const auto range = end - first + 1;

  return str.substr(first, range);
}

void replace_all(std::string &in, const std::string &from, const std::string &to)
{
  if(from.empty()) {
    return;
  }
  size_t start_pos = 0;
  while((start_pos = in.find(from, start_pos)) != std::string::npos) {
    in.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

std::string to_string(const matador::time &x, const char *format)
{
  struct tm timeinfo = x.get_tm();
#ifdef _MSC_VER
  char buffer[255];
  // try to find "%f" for milliseconds
  const char *fpos = strstr(format, "%f");
  if (fpos != nullptr) {
    // split format string (exclude %f)
    size_t len = fpos - format;
    char *d = new char[len + 1];
    strncpy_s(d, len + 1, format, len);
    d[len] = '\0';
    std::string fstr = to_string(x, d) + std::to_string(x.milli_second());
    delete[] d;
	if ((fpos + 2)[0] != '\0') {
	  fstr += to_string(x, fpos + 2);
	}
    return fstr;
    /*
    len = strlen(format) - len - 2;
    d = new char[len + 1];
    strncpy_s(d, len + 1, fpos + 2, len);
    d[len] = '\0';
    fstr += to_string(x, d);
    delete[] d;
    return fstr;
    */
  } else {
    if (strftime(buffer, 255, format, &timeinfo) == 0) {
      throw std::logic_error("couldn't format time string");
    }
    return buffer;
  }
#else
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
#endif
}

std::string to_string(const matador::date &x, const char *format)
{
  time_t now = std::time(nullptr);
  struct tm timeinfo{};

#ifdef _MSC_VER
  localtime_s(&timeinfo, &now);
#else
  localtime_r(&now, &timeinfo);
#endif

  timeinfo.tm_mon = x.month() - 1;
  timeinfo.tm_year = x.year() - 1900;
  timeinfo.tm_mday = x.day();

  char buffer[80];
  if (strftime(buffer, 80, format, &timeinfo) == 0) {
    throw std::logic_error("couldn't format date string");
  }
  return buffer;
}

const char *skip_ws(const char *str)
{
  while(isspace(*str)) {
    str++;
  }
  return str;
}

bool is_eos(char c)
{
  return c == '\0';
}
}
