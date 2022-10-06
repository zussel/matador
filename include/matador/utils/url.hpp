#ifndef MATADOR_URL_HPP
#define MATADOR_URL_HPP

#include "matador/utils/export.hpp"

#include <string>
#include <unordered_map>

namespace matador {
namespace url {

/**
 * URL encodes a given string and returns the
 * encoded string.
 *
 * @param str String to be URL encoded.
 * @return The URL encoded string.
 */
std::string encode(const std::string &str);

/**
 * URL decodes the given string and writes
 * the result into the given string reference.
 * If string could be decoded successfully true
 * is returned.
 *
 * @param str String to be URL decoded.
 * @param decoded The decoded string
 * @return True on successfully decoding
 */
bool decode(const std::string &str, std::string &decoded);

}
}

#endif //MATADOR_URL_HPP
