#ifndef MATADOR_URL_HPP
#define MATADOR_URL_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include <string>
#include <unordered_map>

namespace matador {
namespace http {

class OOS_HTTP_API url
{
public:
  static std::string encode(const std::string &str);
  static bool decode(const std::string &str, std::string &decoded);

private:
  static std::unordered_map<char, const char *> char_to_enc_map_;
};

}
}
#endif //MATADOR_URL_HPP
