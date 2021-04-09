#include "matador/http/template_engine.hpp"

#include "matador/http/detail/template_state_factory.hpp"
#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_state.hpp"
#include "matador/http/detail/template_part.hpp"

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
  detail::template_parser parser;

  parts_.clear();

  rendered_.clear();
  cursor_ = format;

  auto part = parser.parse(cursor_);

  rendered_ = part->render(data);
}

const std::string &template_engine::str() const
{
  return rendered_;
}

}
}
