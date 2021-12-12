#ifndef MATADOR_URL_HPP
#define MATADOR_URL_HPP

#include "matador/utils/export.hpp"

#include <string>
#include <unordered_map>

namespace matador {

class OOS_UTILS_API url
{
public:
  static std::string encode(const std::string &str);
  static bool decode(const std::string &str, std::string &decoded);

private:
  static std::unordered_map<char, const char *> char_to_enc_map_;
};

}
#endif //MATADOR_URL_HPP
