#include "matador/utils/string_cursor.hpp"
#include "matador/utils/string.hpp"

#include <stdexcept>

namespace matador {

string_cursor::string_cursor(const char *str)
  : cursor_(str)
{}

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
