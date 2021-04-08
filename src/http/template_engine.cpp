#include "matador/http/template_engine.hpp"

#include "matador/http/detail/template_state_factory.hpp"
#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_state.hpp"

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
        rendered_ = state_stack_.top()->str();
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

  rendered_ = state_stack_.top()->str();
}

const std::string &template_engine::str() const
{
  return rendered_;
}

void template_engine::handle_variable(const json &data)
{
  // start variable evaluation
  cursor_.next_char();
  std::string token = detail::parse_token(cursor_);

  char c = cursor_.current_char();
  if (c != '}') {
    throw std::logic_error("not a valid token closing bracket");
  }

  state_stack_.top()->on_variable(token, data);
}

void template_engine::handle_command(const json &data)
{
  cursor_.next_char();
  // assume for loop
  std::string cmd = detail::parse_token(cursor_);

  if (state_stack_.top()->is_end_tag(cmd)) {
    state_stack_.top()->execute(data);
    auto result = state_stack_.top()->str();
    state_stack_.pop();
    state_stack_.top()->append(result);
  } else {
    auto cmdptr = detail::template_state_factory::instance().produce(cmd);

    if (!cmdptr) {
      throw std::logic_error("unknown command '" + cmd + "'");
    }

    cmdptr->configure(cursor_);

    state_stack_.push(cmdptr);
  }
}

void execute_foreach(const json &data) {

}

}
}
