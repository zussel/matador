#include "matador/utils/base64.hpp"

namespace matador {

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

const int char_map2[] = {
  62,  0,  62, 0, 64,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
   0,  0,  0,  0,  0,  0,  0,
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25,
   0,  0,  0,  0, 63,  0,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
  37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51
};

const int char_offset = 43;

std::string base64::encode(const std::string &str)
{
  return encode(str.c_str(), str.size());
}

std::string base64::encode(const char *str, size_t size)
{
  return encode(str, size, base64_type::BASE64);
}

std::string base64::encode_url(const std::string &str)
{
  return encode_url(str.c_str(), str.size());
}

std::string base64::encode_url(const char *str, size_t size)
{
  return encode(str, size, base64_type::BASE64URL);
}

void encode_pack(const unsigned char *in, int range_max, const char *codebase, std::string &result);
void decode_pack(const unsigned char *in, std::string &result);

std::string base64::encode(const char *str, size_t size, base64_type type)
{
  std::string result;
  unsigned char pack3[3];
  // 3 byte to 4 chars (3 x 8 bits -> 4 x 6 bits)
  int pack_count = 0;
  for (size_t i = 0; i < size; ++i) {
    pack3[pack_count++] = str[i];

    if (pack_count == 3) {
      encode_pack(pack3, 4, code_base[type], result);
      pack_count = 0;
    }

  }
  if (pack_count) {
    auto i = pack_count;
    while (i < 3) {
      pack3[i++] = '\0';
    }

    encode_pack(pack3, pack_count + 1, code_base[type], result);

    unsigned char fill_char = type == base64_type::BASE64 ? '=' : '.';
    while (pack_count++ < 3) {
      result += fill_char;
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
  std::string result;
  unsigned char pack4[4];

  int pack_count = 0;
  // 4 chars to 3 byte (4 x 6 bits -> 3 x 8 bits)
  for (size_t i = 0; i < size; ++i) {
    pack4[pack_count++] = str[i];

    if (pack_count == 4) {
      decode_pack(pack4, result);
      pack_count = 0;
    }
  }

  return result;
}

void encode_pack(const unsigned char *in, int range_max, const char *codebase, std::string &result)
{
  unsigned char out[4];
  out[0] = (in[0] & 0xfc) >> 2;
  out[1] = ((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4);
  out[2] = ((in[1] & 0x0f) << 2) + ((in[2] & 0xc0) >> 6);
  out[3] = (in[2] & 0x3f);

  for (auto i = 0; i < range_max; ++i) {
    result += codebase[out[i]];
  }
}

void decode_pack(const unsigned char *in, std::string &result)
{
  unsigned char out[3];

  out[0] = (char_map2[in[0] - char_offset] << 2) + ((char_map2[in[1] - char_offset] & 0x30) >> 4);
  out[1] = ((char_map2[in[1] - char_offset] & 0x0f) << 4) + ((char_map2[in[2] - char_offset] & 0x3c) >> 2);
  out[2] = ((char_map2[in[2] - char_offset] & 0x03) << 6) + (char_map2[in[3] - char_offset]);

  for (unsigned char o : out) {
    if (o == 0) {
      continue;
    }
    result += o;
  }
}

}