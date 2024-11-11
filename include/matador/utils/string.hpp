#ifndef STRING_HPP
#define STRING_HPP

#include "matador/utils/export.hpp"
#include "matador/utils/types.hpp"

#include <vector>
#include <list>
#include <string>
#include <type_traits>

namespace matador {

class time;

class date;

}

namespace matador::utils {

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
OOS_UTILS_API size_t split(const std::string &str, char delim, std::vector<std::string> &values);

/**
 * Splits a string by a delimiter and
 * add the string tokens to a vector. The
 * size of the vector is returned.
 *
 * @param str The string to split.
 * @param delim The delimiter character.
 * @return The vector with split strings.
 */
OOS_UTILS_API std::vector<std::string> split(const std::string &str, char delim);

/**
 * Splits a string by a delimiter and
 * add the string tokens to a list. The
 * size of the list is returned.
 *
 * @param str The string to split.
 * @param delim The delimiter character.
 * @param values The result list.
 * @return The size of the list.
 */
OOS_UTILS_API size_t split(const std::string &str, char delim, std::list<std::string> &values);

/**
 * @fn std::string trim(const std::string& str, const std::string&)
 * Trims a string by removing leading and trailing characters
 * The default characters are spaces and tabs
 *
 * @param str The string to be trimmed
 * @param whitespace The trimming characters
 * @return the trimmed string
 */
OOS_UTILS_API std::string trim(const std::string& str, const std::string& whitespace = " \t");

/**
 * Replaces all occurrences of string from in given string
 * with string to.
 *
 * @param in Source string where the replacement takes place
 * @param from The string to be replaced
 * @param to The new string
 */
OOS_UTILS_API void replace_all(std::string &in, const std::string &from, const std::string &to);

/**
 * The date_format struct represents
 * common date format string as ISO8601
 * and the likes.
 */
struct OOS_UTILS_API date_format
{
#ifdef _MSC_VER
	static const char* ISO8601;
#else
  /**
   * date format string representing the ISO8601 format
   */
  static constexpr const char* ISO8601 = "%Y-%m-%d";
#endif
};

/**
 * The time_format struct represents
 * common time format string as ISO8601
 * and the likes.
 */
struct OOS_UTILS_API time_format
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
OOS_UTILS_API std::string to_string(const matador::time &x, const char *format = time_format::ISO8601);

/**
 * Converts a date object with a given format
 * to a string
 *
 * @param x Date object to convert.
 * @param format The format string
 */
OOS_UTILS_API std::string to_string(const matador::date &x, const char *format = date_format::ISO8601);

OOS_UTILS_API const std::string& to_string(const std::string &str);
OOS_UTILS_API std::string to_string(const blob &data);

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
std::string to_string(T x, size_t precision = 0, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr)
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

/**
 * Skip all whitespace characters (tab, spaces, etc.)
 * for the given string and return the new position
 *
 * @param str String to skip whitespace characters in
 * @return First non whitespace positon of string
 */
OOS_UTILS_API const char* skip_ws(const char *str);

/**
 * Checks if the given character is end of string.
 *
 * @param c Character to check
 * @return True if character is end of string
 */
OOS_UTILS_API bool is_eos(char c);

/**
 * Joins a range of elements as string within a list
 * with a given delimiter and writes it to the
 * given stream
 *
 * @tparam R Type og the range (e.g. map, list, vector, etc)
 * @param range The range with the elements to join_left
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class R >
std::string join(R &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (const auto &i : range) {
      result += to_string(i);
    }
  } else {
    auto it = range.begin();
    result += to_string(*it++);
    for (;it != range.end(); ++it) {
      result += delim + to_string(*it);
    }
  }
  return result;
}

}

#endif /* STRING_HPP */
