/*
 * Updated to C++, zedwood.com 2012
 * Based on Olivier Gay's version
 * See Modified BSD License below:
 *
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Issue date:  04/30/2005
 * http://www.ouah.org/ogay/sha2/
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 */
#ifndef MATADOR_SHA256_HPP
#define MATADOR_SHA256_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <string>

namespace matador {
namespace ext {

/// @cond MATADOR_DEV

class OOS_UTILS_API SHA256
{
protected:
  typedef unsigned char uint8;
  typedef unsigned int uint32;
  typedef unsigned long long uint64;

  const static uint32 sha256_k[];
  static const unsigned int SHA224_256_BLOCK_SIZE = (512/8);
public:
  void init();
  void update(const unsigned char *message, unsigned int len);
  void final(unsigned char *digest);
  static const unsigned int DIGEST_SIZE = (256 / 8);
  static const unsigned int RESULT_SIZE = 65;

protected:
  void transform(const unsigned char *message, unsigned int block_nb);
  unsigned int m_tot_len;
  unsigned int m_len;
  unsigned char m_block[2*SHA224_256_BLOCK_SIZE];
  uint32 m_h[8];
};

/// @endcond

/**
 * @brief SH256 encrypting function
 *
 * Encrypts a given string with sha256 algorithm
 * and returns the encrypted data as string
 *
 * @param input String to encrypt
 * @return The encrypted data as string
 */
OOS_UTILS_API std::string sha256(const std::string& input);
OOS_UTILS_API std::string sha256(const char *input);

/**
 * @brief SH256 encrypting function
 *
 * Encrypts a given character string with sha256 algorithm
 * and returns the encrypted data as string

 * @param input Character string to encrypt
 * @param length Length of the character string
 * @return The encrypted data as string
 */
OOS_UTILS_API std::string sha256(const char *input, size_t length);

/**
 * @brief SH256 encrypting function
 *
 * Encrypts a given character string with sha256 algorithm
 * and write the encrypted data into given array.

 * @param input Character string to encrypt
 * @param length Length of the character string
 * @param digest Array to write the encrypted data to
 * @param hash_size Size of the array
 */
OOS_UTILS_API void sha256(const char *input, size_t length, unsigned char *digest, size_t hash_size);

/// @cond MATADOR_DEV

#define SHA2_SHFR(x, n)    ((x) >> (n))
#define SHA2_ROTR(x, n)   (((x) >> (n)) | ((x) << ((sizeof(x) << 3) - (n))))
#define SHA2_ROTL(x, n)   (((x) << (n)) | ((x) >> ((sizeof(x) << 3) - (n))))
#define SHA2_CH(x, y, z)  (((x) & (y)) ^ (~(x) & (z)))
#define SHA2_MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))
#define SHA2_UNPACK32(x, str)                 \
{                                             \
    *((str) + 3) = (uint8) ((x)      );       \
    *((str) + 2) = (uint8) ((x) >>  8);       \
    *((str) + 1) = (uint8) ((x) >> 16);       \
    *((str) + 0) = (uint8) ((x) >> 24);       \
}
#define SHA2_PACK32(str, x)                   \
{                                             \
    *(x) =   ((uint32) *((str) + 3)      )    \
           | ((uint32) *((str) + 2) <<  8)    \
           | ((uint32) *((str) + 1) << 16)    \
           | ((uint32) *((str) + 0) << 24);   \
}

/// @endcond

}
}
#endif //MATADOR_SHA256_HPP
