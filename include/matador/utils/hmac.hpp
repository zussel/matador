#ifndef MATADOR_HMAC_HPP
#define MATADOR_HMAC_HPP

#include "matador/utils/export.hpp"

#include <string>

namespace matador {

OOS_UTILS_API std::string hmac(const std::string &key, const std::string &message);
OOS_UTILS_API std::string hmac(const char *key, size_t keylen, const char *message, size_t msglen);

}

#endif //MATADOR_HMAC_HPP
