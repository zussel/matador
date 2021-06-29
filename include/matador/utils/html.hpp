#ifndef MATADOR_HTML_HPP
#define MATADOR_HTML_HPP

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

class OOS_UTILS_API html
{
public:
  static std::string escape(const std::string &value);
};

}
#endif //MATADOR_HTML_HPP
