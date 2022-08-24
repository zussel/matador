#ifndef MATADOR_HTML_HPP
#define MATADOR_HTML_HPP

#include "matador/utils/export.hpp"

#include <string>

namespace matador {

class OOS_UTILS_API html
{
public:
  static std::string escape(const std::string &value);
};

}
#endif //MATADOR_HTML_HPP
