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

/**
 * @fn std::string trim(const std::string& str, const std::string&)
 * Trims a string by removing leading and trailing characters
 * The default characters are spaces and tabs
 *
 * @param str The string to be trimmed
 * @param whitespace The trimming characters
 * @return the trimmed string
 */
OOS_API std::string trim(const std::string& str, const std::string& whitespace = " \t");

/**
 * Replaces all occurences of string from in given string
 * with string to.
 *
 * @param in Source string where the replacement takes place
 * @param from The string to be replaced
 * @param to The new string
 */
OOS_API void replace_all(std::string &in, const std::string &from, const std::string &to);

/**
 * The date_format struct represents
 * common date format string as ISO8601
 * and the likes.
 */
struct OOS_API date_format
{
#ifdef _MSC_VER
	static const char* ISO8601;
#else
  /**
   * date format string representing the ISO8601 format
   */
  static constexpr const char* ISO8601 = "%F";
#endif
};

/**
 * The time_format struct represents
 * common time format string as ISO8601
 * and the likes.
 */
struct OOS_API time_format
{
#ifdef _MSC_VER
  static const char* ISO8601;
#else
  /**
   * time format string representing the ISO8601 format
   */
  static constexpr const char* ISO8601 = "%FT%T";
#endif
};

/**
 * Converts a time object with a given format
 * to a string
 *
 * @param x Time object to convert.
 * @param format The format string
 */
OOS_API std::string to_string(const oos::time &x, const char *format = time_format::ISO8601);

/**
 * Converts a date object with a given format
 * to a string
 *
 * @param x Date object to convert.
 * @param format The format string
 */
OOS_API std::string to_string(const oos::date &x, const char *format = date_format::ISO8601);

/**
 * Convert any floating point values
 * into a string with a given precision.
 *
 * @tparam T The floating point type
 * @param x The value to be converted.
 * @param precision The precision to use for converting
 * @return The floating point value as string
 */
template < class T >
std::string to_string(T x, size_t precision = 0, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
{
  if (precision == 0) {
    return to_string(x);
  } else {
    char format[32];
#ifdef _MSC_VER
    sprintf_s(format, "%%.%df", (int)precision);
#else
    sprintf(format, "%%.%df", (int)precision);
#endif
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
