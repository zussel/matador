#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_part.hpp"
#include "matador/http/detail/template_state_factory.hpp"

#include "matador/utils/string.hpp"

namespace matador {
namespace http {
namespace detail {

std::string parse_token(string_cursor &cursor)
{
  std::string token;
  char c = cursor.skip_whitespace();

  while(!is_eos(c)) {
    if (isalnum(c) || c == '.') {
      token.push_back(c);
    } else {
      break;
    }
    c = cursor.next_char();
  }

  cursor.skip_whitespace();
  return token;
}

bool parse_end_of_command_tag(string_cursor &cursor)
{
  if (cursor.skip_whitespace() != '%') {
    return false;
  } else if (cursor.next_char() != '}') {
    return false;
  } else {
    cursor.next_char();
  }
  return true;
}

}
}
}