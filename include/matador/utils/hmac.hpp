#ifndef MATADOR_HMAC_HPP
#define MATADOR_HMAC_HPP

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

OOS_UTILS_API std::string hmac(const std::string &key, const std::string &message);
OOS_UTILS_API std::string hmac(const char *key, size_t keylen, const char *message, size_t msglen);

}

#endif //MATADOR_HMAC_HPP
