#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <vector>
#include <string>

namespace oos {
  
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
size_t split(const std::string &str, char delim, std::vector<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}
}

#endif /* STRING_HPP */
