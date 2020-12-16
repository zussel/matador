#ifndef MATADOR_BASE64_HPP
#define MATADOR_BASE64_HPP

#include <string>

namespace matador {

class base64
{
public:
  static std::string encode(const std::string &str);
  static std::string encode(const char *str, size_t size);
  static std::string decode(const std::string &str);
  static std::string decode(const char *str, size_t size);
};

}
#endif //MATADOR_BASE64_HPP
