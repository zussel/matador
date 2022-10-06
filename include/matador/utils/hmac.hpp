#ifndef MATADOR_HMAC_HPP
#define MATADOR_HMAC_HPP

#include "matador/utils/export.hpp"

#include <string>

namespace matador {

/**
 * Encodes the given string message with the given key and
 * returns the encoded string.
 *
 * @param key Key to be used for encoding.
 * @param message Message to be encoded.
 * @return The encoded message.
 */
OOS_UTILS_API std::string hmac(const std::string &key, const std::string &message);

/**
 * Encodes the given string message with the given key and
 * returns the encoded string.
 *
 * @param key Key to be used for encoding.
 * @param keylen Length of the key.
 * @param message Message to be encoded.
 * @param msglen Length of the message.
 * @return The encoded message.
 */
OOS_UTILS_API std::string hmac(const char *key, size_t keylen, const char *message, size_t msglen);

}

#endif //MATADOR_HMAC_HPP
