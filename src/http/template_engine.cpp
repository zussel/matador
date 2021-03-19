#include "matador/http/template_engine.hpp"

#include "matador/http/detail/template_state_factory.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/string.hpp"

namespace matador {
namespace http {

// {% [command] %}
// {{ [key] }}
// {{ name }} -> replace

// {% include <file> %}

// {% extends <file> %}

// {% block <name> %}...{% endblock %}

// {% for elem in list %}
// copy ...
// {% endfor %}


void template_engine::render(const std::string &format, const json &data)
{
  render(format.c_str(), format.size(), data);
}

void template_engine::render(const char *format, size_t len, const json &data)
{
  while (!state_stack_.empty()) {
    state_stack_.pop();
  }

  state_stack_.push(detail::template_state_factory::instance().produce("global"));
  rendered_.clear();
  cursor_ = format;
  // copy until next '{' is found
  char c = cursor_.current_char();
  while (!is_eos(c)) {
    // find begin of variable/command tag
    while (c != '{') {
      if (is_eos(c)) {
        return;
      }
      state_stack_.top()->append(c);
      c = cursor_.next_char();
    }
    c = cursor_.next_char();
    if (c == '{') {
      handle_variable(data);
    } else if (c == '%') {
      // start command evaluation
      handle_command(data);
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

void template_engine::handle_variable(const json &data)
{
  // start variable evaluation
  cursor_.next_char();
  std::string token = parse_token();

  const auto &j = data.at_path(token, '.');
  state_stack_.top()->append(j.as<std::string>());
  char c = cursor_.current_char();
  if (c != '}') {
    throw std::logic_error("not a valid token closing bracket");
  }
}

void template_engine::handle_command(const json &data)
{
  cursor_.next_char();
  // assume for loop
  std::string cmd = parse_token();

  cursor_.skip_whitespace();

  std::string elem_name = parse_token();

  cursor_.skip_whitespace();

  std::string in = parse_token();

  if (in != "in") {
    throw std::logic_error("expected keyword 'in' for for loop");
  }

  cursor_.skip_whitespace();

  std::string list_name = parse_token();

  const json &cont = data.get(elem_name);

  if (!cont.is_object() || !cont.is_array()) {
    throw std::logic_error("json object isn't of type array or object");
  }


  // validate


  // store foreach content

  // after endfor execute command
}

void execute_foreach(const json &data) {

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
