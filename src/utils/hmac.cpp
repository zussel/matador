#include "matador/utils/hmac.hpp"
#include "matador/utils/sha256.hpp"

#include <cstring>
#include <vector>

namespace matador {

std::string hmac(const std::string &key, const std::string &message)
{
  return hmac(key.data(), key.length(), message.data(), message.length());
}

std::string hmac(const char *key, size_t keylen, const char *message, size_t msglen)
{
  const unsigned long BLOCK_SIZE = 64;
  const unsigned long HASH_SIZE = 32;

  char keystr[BLOCK_SIZE] {};

  // prepare keystati
  if (keylen > BLOCK_SIZE) {
    auto enc = ext::sha256(key);
    memcpy(keystr, enc.data(), BLOCK_SIZE);
  } else if (keylen < BLOCK_SIZE) {
#ifdef _MSC_VER
    strcpy_s(keystr, BLOCK_SIZE, key);
#else
    strcpy(keystr, key);
#endif
    memset(keystr + keylen, '\0', BLOCK_SIZE - keylen);
  } else {
    memcpy(keystr, key, keylen);
  }

  char o_key_pad[BLOCK_SIZE] {}; //setup the o_key_pad
  char i_key_pad[BLOCK_SIZE] {}; //setup the i_key_pad
  memset(i_key_pad, 0x36, BLOCK_SIZE);
  memset(o_key_pad, 0x5c, BLOCK_SIZE);

  for (unsigned int i = 0; i < BLOCK_SIZE; ++i) {
    o_key_pad[i] ^= keystr[i];
    i_key_pad[i] ^= keystr[i];
  }

  const size_t tmplen = BLOCK_SIZE + msglen;
  std::vector<char> tmp1(BLOCK_SIZE + tmplen);
  //char tmp1[BLOCK_SIZE + tmplen];

  memcpy(tmp1.data(), i_key_pad, BLOCK_SIZE);
  memcpy(tmp1.data() + BLOCK_SIZE, message, msglen);

  unsigned char h1[HASH_SIZE] {};
  ext::sha256(tmp1.data(), tmplen, h1, HASH_SIZE);

  char tmp2[BLOCK_SIZE * HASH_SIZE];

  memcpy(tmp2, o_key_pad, BLOCK_SIZE);
  memcpy(tmp2 + BLOCK_SIZE, h1, HASH_SIZE);

  auto result = ext::sha256(tmp2, BLOCK_SIZE + HASH_SIZE);
  return result;
}
}
