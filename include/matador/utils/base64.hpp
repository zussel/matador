#ifndef MATADOR_BASE64_HPP
#define MATADOR_BASE64_HPP

#include "matador/utils/export.hpp"

#include <string>

namespace matador {

/**
 * @brief Base64 encode and decode class
 *
 * This class encodes and decodes
 * a string of base64 algorithm
 */
class OOS_UTILS_API base64
{
public:

  /**
   * Base64 encodes a given string
   *
   * @param str String to encode
   * @return The encoded string
   */
  static std::string encode(const std::string &str);

  /**
   * Base64 encodes a given character string
   *
   * @param str String to encode
   * @param size Length of the string
   * @return The encoded string
   */
  static std::string encode(const char *str, size_t size);

  /**
   * URL base64 encodes a given string
   *
   * @param str String to encode
   * @return The encoded string
   */
  static std::string encode_url(const std::string &str);

  /**
   * Url base64 encodes a given character string
   *
   * @param str String to encode
   * @param size Length of the string
   * @return The encoded string
   */
  static std::string encode_url(const char *str, size_t size);

  /**
   * Base64 decodes a given string
   *
   * @param str Base64 encoded string to decode
   * @return The decoded string
   */
  static std::string decode(const std::string &str);

  /**
   * Base64 decodes a given character string
   *
   * @param str Base64 encoded string to decode
   * @param size Length of the string
   * @return The decoded string
   */
  static std::string decode(const char *str, size_t size);

private:
  enum base64_type {
    BASE64, BASE64URL
  };

  static std::string encode(const char *str, size_t size, base64_type type);
};

}
#endif //MATADOR_BASE64_HPP
