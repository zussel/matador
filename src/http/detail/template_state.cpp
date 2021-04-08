#include "matador/http/detail/template_state.hpp"
#include "matador/http/detail/template_parser.hpp"

#include "matador/http/template_engine.hpp"

#include "matador/utils/string_cursor.hpp"
#include "matador/utils/json.hpp"

#include <stdexcept>

namespace matador {
namespace http {
namespace detail {

void global_state::execute(const json&)
{
}

bool global_state::is_executable() const
{
  return false;
}

bool global_state::is_end_tag(const std::string&) const
{
  return false;
}

void global_state::append(const std::string &part)
{
  rendered_.append(part);
}

void global_state::append(char c)
{
  rendered_ += c;
}

std::string global_state::str() const
{
  return rendered_;
}

void global_state::configure(string_cursor &)
{
  rendered_.clear();
}

void global_state::on_variable(const std::string &variable, const json &data)
{
  const auto &j = data.at_path(variable, '.');
  append(j.as<std::string>());
}

void foreach_state::configure(string_cursor &cursor)
{
  repeatable_.clear();
  content_.clear();

  cursor.skip_whitespace();

  elem_name_ = detail::parse_token(cursor);

  cursor.skip_whitespace();

  std::string in = detail::parse_token(cursor);

  if (in != "in") {
    throw std::logic_error("expected keyword 'in' for for loop");
  }

  cursor.skip_whitespace();

  list_name_ = detail::parse_token(cursor);
}

bool foreach_state::is_executable() const
{
  return false;
}

bool foreach_state::is_end_tag(const std::string &tag) const
{
  return tag == "endfor";
}

void foreach_state::execute(const json &data)
{
  const json &cont = data.get(list_name_);

  if (!cont.is_object() && !cont.is_array()) {
    throw std::logic_error("json object isn't of type array or object");
  }

  template_engine engine;

  for(const auto &elem : cont) {

    json item = json::object();
    item[elem_name_] = elem;

    engine.render(repeatable_, item);
    content_ += engine.str();
  }
}

void foreach_state::append(const std::string &str)
{
  repeatable_.append(str);
}

void foreach_state::append(char c)
{
  repeatable_ += c;
}

std::string foreach_state::str() const
{
  return content_;
}

void foreach_state::on_variable(const std::string &variable, const json &)
{
  append("{{");
  append(variable);
  append("}}");
}

}
}
}
