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

struct date_format
{
  static constexpr const char* ISO8601 = "%F";
};

struct time_format
{
  static constexpr const char* ISO8601 = "%F %T";
};

OOS_API std::string to_string(const oos::time &x, const char *format = time_format::ISO8601);
OOS_API std::string to_string(const oos::date &x, const char *format = date_format::ISO8601);

}

#endif /* STRING_HPP */
