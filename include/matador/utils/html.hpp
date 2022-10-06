#ifndef MATADOR_HTML_HPP
#define MATADOR_HTML_HPP

#include "matador/utils/export.hpp"

#include <string>

namespace matador {

/**
 * Html utils
 */
class OOS_UTILS_API html
{
private:
  html() = default;

public:
  /**
   * Method html escapes a given string and
   * returns the result.
   *
   * @param value String to be html escaped
   * @return The html escaped string
   */
  static std::string escape(const std::string &value);
};

}
#endif //MATADOR_HTML_HPP
