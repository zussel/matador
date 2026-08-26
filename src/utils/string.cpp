#include "matador/utils/string.hpp"

#include <sstream>

namespace matador::utils {
std::string to_string(const blob_type_t& data) {
  static constexpr char HEXITS[] = "0123456789ABCDEF";

  std::string str(2 * data.size(), '\0');
  auto item = str.begin();

  for(const auto c : data) {
    *item++ = HEXITS[c >> 4];
    *item++ = HEXITS[c & 0x0F];
  }

  return str;
}

std::string to_string(const date_type_t &data) {
  char buf[11]; // "YYYY-MM-DD" + '\0'

  std::snprintf(buf, sizeof(buf), "%04d-%02u-%02u",
    data.year, static_cast<unsigned>(data.month), static_cast<unsigned>(data.day));

  return buf;
}

std::string to_string(const time_type_t &data) {
  char buf[16]; // "HH:MM:SS.ffffff" + '\0'

  std::snprintf(buf, sizeof(buf), "%02u:%02u:%02u.%06u",
    static_cast<unsigned>(data.hour), static_cast<unsigned>(data.minute), static_cast<unsigned>(data.second), data.microsecond);

  return buf;
}

size_t split(const std::string &str, char delim, std::vector<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}

std::vector<std::string> split(const std::string &str, char delim)
{
  std::stringstream ss(str);
  std::string item;
  std::vector<std::string> result;
  while (std::getline(ss, item, delim)) {
    result.push_back(item);
  }
  return result;
}

size_t split(const std::string &str, const char delim, std::list<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}

#ifdef _MSC_VER
// const char* date_format::ISO8601 = "%Y-%m-%d";
// const char* time_format::ISO8601 = "%Y-%m-%dT%H:%M:%S";
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

}
