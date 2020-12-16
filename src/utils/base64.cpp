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
  return encode(str.c_str(), str.size());
}

std::string base64::encode(const char *str, size_t size)
{
  std::string result;
  char pack3[3];
  char pack4[4];
  // 3 byte to 4 chars (3 x 8 bits -> 4 x 6 bits)
  for (size_t i = 0; i < size; ++i) {

  }
  return result;
}

std::string base64::decode(const std::string &str)
{
  return decode(str.c_str(), str.size());
}

std::string base64::decode(const char *str, size_t size)
{
  return str;
}
}