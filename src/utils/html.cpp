#include "matador/utils/html.hpp"

namespace matador {

std::string html::escape(const std::string &value)
{
  std::string result;
  result.reserve((size_t)((float)value.size() * 1.1));
  for (size_t pos = 0; pos != value.size(); ++pos) {
    switch (value[pos]) {
      case '&':
        result.append("&amp;");
        break;
      case '\"':
        result.append("&quot;");
        break;
      case '\'':
        result.append("&apos;");
        break;
      case '<':
        result.append("&lt;");
        break;
      case '>':
        result.append("&gt;");
        break;
      default:
        result.append(&value[pos], 1);
        break;
    }
  }
  return result;
}

}
