#include "matador/http/template_engine.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/string.hpp"

namespace matador {
namespace http {

// {% [command] %}
// {{ [key] }}
// {{ name }} -> replace

void template_engine::render(const std::string &format, const json &data)
{
  render(format.c_str(), format.size(), data);
}

void template_engine::render(const char *format, size_t len, const json &data)
{
  rendered_.clear();
  cursor_ = format;
  // copy until next '{' is found
  char c = cursor_.current_char();
  while (!is_eos(c)) {
    while (c != '{') {
      if (is_eos(c)) {
        return;
      }
      rendered_ += c;
      c = cursor_.next_char();
    }
    c = cursor_.next_char();
    if (c == '{') {
      // start variable evaluation
      cursor_.next_char();
      std::string token = parse_token();

      const auto &j = data.at_path(token, '.');
      rendered_ += j.as<std::string>();
      c = cursor_.current_char();
      if (c != '}') {
        throw std::logic_error("not a valid token closing bracket");
      }
    } else if (c == '%') {
      // start command evaluation
    } else {
      continue;
    }

    c = cursor_.next_char();

    if (c != '}') {
      throw std::logic_error("not a valid token closing bracket");
    }

    c = cursor_.next_char();
  }
}

const std::string &template_engine::str() const
{
  return rendered_;
}

std::string template_engine::parse_token()
{
  std::string token;
  char c = cursor_.skip_whitespace();

  while(!is_eos(c)) {
    if (isspace(c)) {
      // token is finish
      break;
    } else if (isalnum(c) || c == '.') {
      token.push_back(c);
    }
    c = cursor_.next_char();
  }

  cursor_.skip_whitespace();
  return token;
}

string_cursor &string_cursor::operator=(const char *str)
{
  cursor_ = str;
  return *this;
}

char string_cursor::skip_whitespace()
{
  cursor_ = skip_ws(cursor_);
  return cursor_[0];
}

char string_cursor::next_char()
{
  if (is_null()) {
    throw std::logic_error("no current string");
  }
  return (++cursor_)[0];
}

char string_cursor::current_char() const
{
  return cursor_[0];
}
}
}
