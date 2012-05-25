#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <vector>
#include <string>

namespace oos {
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