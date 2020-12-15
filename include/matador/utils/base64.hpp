#ifndef MATADOR_BASE64_HPP
#define MATADOR_BASE64_HPP

#include <string>

namespace matador {

class base64
{
public:
  static std::string encode(const std::string &str);
  static std::string decode(const std::string &str);
};

}
#endif //MATADOR_BASE64_HPP
