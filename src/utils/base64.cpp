#include "matador/utils/base64.hpp"

namespace matador {

enum base64_type {
  BASE64, BASE64URL
};

const char *code_base[2] = {
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789"
  "+/",

  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789"
  "-_"
};

std::string base64::encode(const std::string &str)
{
  return str;
}

std::string base64::decode(const std::string &str)
{
  return str;
}

}