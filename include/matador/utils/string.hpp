#ifndef STRING_HPP
#define STRING_HPP

#include "matador_export.h"

#include "matador/utils/convert.hpp"
#include "matador/utils/types.hpp"

#include <list>
#include <string>

namespace matador::utils {

/**
 * Converts each byte of the given binary data
 * into is hex string representation and return
 * all bytes of blob as string.
 *
 * @param data Binary data to be converted
 * @return Binary data as string
 */
MATADOR_EXPORT std::string to_string(const blob_type_t &data);
MATADOR_EXPORT std::string to_string(const date_type_t &data);
MATADOR_EXPORT std::string to_string(const time_type_t &data);

template <typename IntegerType>
std::string to_hex_string(IntegerType data, const size_t width = sizeof(IntegerType)<<1) {
  static auto digits = "0123456789ABCDEF";
  std::string result(width,'0');
  for (size_t i=0, j=(width-1)*4 ; i<width; ++i,j-=4) {
    result[i] = digits[(data>>j) & 0x0f];
  }
  return result;
}

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
MATADOR_EXPORT size_t split(const std::string &str, char delim, std::vector<std::string> &values);

/**
 * Splits a string by a delimiter and
 * add the string tokens to a vector. The
 * size of the vector is returned.
 *
 * @param str The string to split.
 * @param delim The delimiter character.
 * @return The vector with split strings.
 */
MATADOR_EXPORT std::vector<std::string> split(const std::string &str, char delim);

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
MATADOR_EXPORT size_t split(const std::string &str, char delim, std::list<std::string> &values);

/**
 * @fn std::string trim(const std::string& str, const std::string&)
 * Trims a string by removing leading and trailing characters
 * The default characters are spaces and tabs
 *
 * @param str The string to be trimmed
 * @param whitespace The trimming characters
 * @return the trimmed string
 */
MATADOR_EXPORT std::string trim(const std::string& str, const std::string& whitespace = " \t");

/**
 * Replaces all occurrences of string from in given string
 * with string to.
 *
 * @param in Source string where the replacement takes place
 * @param from The string to be replaced
 * @param to The new string
 */
MATADOR_EXPORT void replace_all(std::string &in, const std::string &from, const std::string &to);

template <typename Type >
std::string to_string(const Type &value) {
   const auto res = to<std::string>(value);
   if (res.is_ok()) {
     return *res;
   }
   return "";
}

/**
 * Joins a range of elements as string within a list
 * with a given delimiter and writes it to the
 * given stream
 *
 * @tparam Range Type og the range (e.g. map, list, vector, etc)
 * @param range The range with the elements to join_left
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class Range >
std::string join(const Range &range, const std::string &delim) {
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
#endif //STRING_HPP
