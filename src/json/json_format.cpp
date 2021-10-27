#include "matador/json/json_format.hpp"

namespace matador {

json_format json_format::compact { false, true, 0 }; /* NOLINT */
json_format json_format::pretty { true, false, 2 }; /* NOLINT */

inline json_format::json_format(bool enable_line_break)
: enable_line_break_(enable_line_break)
{}

inline json_format::json_format(unsigned indentation)
: indentation_(indentation)
{}

inline json_format::json_format(bool enable_line_break, bool skip_empty, unsigned indentation)
: enable_line_break_(enable_line_break)
, skip_empty_(skip_empty)
, indentation_(indentation)
{}

bool json_format::show_line_break() const
{
  return enable_line_break_;
}

bool json_format::skip_empty()
{
  return skip_empty_;
}

unsigned json_format::indentation() const
{
  return indentation_;
}

char json_format::indentation_char() const
{
  return indentation_char_;
}

}
