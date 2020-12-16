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
  unsigned char pack3[3];
  unsigned char pack4[4];
  // 3 byte to 4 chars (3 x 8 bits -> 4 x 6 bits)
  int pack_count = 0;
  for (size_t i = 0; i < size; ++i) {
    pack3[pack_count++] = str[i];

    if (pack_count == 3) {
      pack4[0] = (pack3[0] & 0xfc) >> 2;
      pack4[1] = ((pack3[0] & 0x03) << 4) + ((pack3[1] & 0xf0) >> 4);
      pack4[2] = ((pack3[1] & 0x0f) << 2) + ((pack3[2] & 0xc0) >> 6);
      pack4[3] = (pack3[2] & 0x3f);

      for (pack_count = 0; pack_count < 4; ++pack_count) {
        result += code_base[0][pack4[pack_count]];
      }
      pack_count = 0;
    }

  }
  if (pack_count) {
    auto i = pack_count;
    while (i < 3) {
      pack3[i++] = '\0';
    }

    pack4[0] = (pack3[0] & 0xfc) >> 2;
    pack4[1] = ((pack3[0] & 0x03) << 4) + ((pack3[1] & 0xf0) >> 4);
    pack4[2] = ((pack3[1] & 0x0f) << 2) + ((pack3[2] & 0xc0) >> 6);
    pack4[3] = (pack3[2] & 0x3f);

    for (i = 0; i < pack_count + 1; ++i) {
      result += code_base[0][pack4[i]];
    }

    while (pack_count++ < 3) {
      result += '=';
    }
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