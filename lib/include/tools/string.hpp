#ifndef STRING_HPP
#define STRING_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include <sstream>
#include <vector>
#include <string>
#include <type_traits>

namespace oos {

class time;
class date;

/**
 * Splits a string by a delimiter and
 * add the string tokens to a vector. The
 * size of the vector is returned.
 * 
 * @param str The string to split.
 * @param delim The delimiter character.
 * @param values The result vector.
 * @return The size of the vector.
 */
OOS_API size_t split(const std::string &str, char delim, std::vector<std::string> &values);
OOS_API std::string trim(const std::string& str, const std::string& whitespace = " \t");

struct OOS_API date_format
{
#ifdef _MSC_VER
	static const char* ISO8601;
#else
  static constexpr const char* ISO8601 = "%F";
#endif
};

struct OOS_API time_format
{
#ifdef _MSC_VER
  static const char* ISO8601;
#else
  static constexpr const char* ISO8601 = "%F %T";
#endif
};

OOS_API std::string to_string(const oos::time &x, const char *format = time_format::ISO8601);
OOS_API std::string to_string(const oos::date &x, const char *format = date_format::ISO8601);

template < class T >
std::string to_string(T x, int precision = -1, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
{
  if (precision < 0) {
    return to_string(x);
  } else {
    char format[32];
    sprintf(format, "%%.%df", precision);
    std::string s(32, '\0');
#ifdef _MSC_VER
    auto written = _snprintf_s(&s[0], s.size(), 32, format, x);
#else
    auto written = std::snprintf(&s[0], s.size(), format, x);
#endif
    s.resize(written);
    return s;
  }
}

}

#endif /* STRING_HPP */
