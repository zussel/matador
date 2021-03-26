#include "matador/http/detail/template_parser.hpp"

#include "matador/utils/string_cursor.hpp"
#include "matador/utils/string.hpp"

namespace matador {
namespace http {
namespace detail {

std::string parse_token(string_cursor &cursor)
{
  std::string token;
  char c = cursor.skip_whitespace();

  while(!is_eos(c)) {
    if (isspace(c)) {
      // token is finish
      break;
    } else if (isalnum(c) || c == '.') {
      token.push_back(c);
    }
    c = cursor.next_char();
  }

  cursor.skip_whitespace();
  return token;
}

}
}
}