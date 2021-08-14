#include "matador/http/template_engine.hpp"

#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_part.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/string_cursor.hpp"

namespace matador {
namespace http {

// {% [command] %}
// {{ [key] }}
// {{ name }} -> replace

// {% include <file> %}

// {% extends <file> %}

// {% block <name> %}...{% endblock %}

// {% for elem in list %}
// loop over elements ...
// {% empty %}
// on empty list
// {% endfor %}

// {% if expression %}
// do this
// {% elif expression %}
// do that
// {% else %}
// do this
// {% fi %}

std::shared_ptr<detail::template_part> template_engine::build(const std::string &format)
{
  return build(format.c_str(), format.size());
}

std::shared_ptr<detail::template_part> template_engine::build(const char *format, size_t len)
{
  detail::template_parser parser;

  string_cursor cursor(format);

  return parser.parse(cursor, [](const std::string&, std::unique_ptr<detail::multi_template_part>&) { return detail::template_parser::NEXT_COMMAND; });
}

std::string template_engine::render(const std::shared_ptr<detail::template_part>& part, const json &data)
{
  return part->render(data);
}

std::string template_engine::render(const std::string &format, const json &data)
{
  return render(build(format), data);
}

std::string template_engine::render(const char *format, size_t len, const json &data)
{
  return render(build(format, len), data);
}

}
}
