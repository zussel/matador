#include "matador/logger/log_manager.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/response.hpp"
#include "matador/http/template_engine.hpp"

using namespace matador;

int main(int /*argc*/, char* /*argv*/[])
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  static std::string index_html { R"(
<html>
<head>
  <title>{{title}}</title>
</head>
<body>
<h1>Hello {{ user.name }}</h1>
<ul>
{% for person in persons %}
  <li>Person {{ person.name }} (id: {{ person.id }})</li>
{% endfor %}
</ul>
</body>
</html>)" };

  json data = {
    { "title", "My first dynamic page" },
    { "user", {
      { "id", 4711 },
      { "name", "George" }
    }},
    { "persons", { {
      {"name", "George"},
      {"id", 2 }
    }, {
      {"name", "Jane"},
      {"id",  1 }
    }, {
      {"name", "Otto"},
      {"id", 3 }
    } } }
  };

  http::server server(9081);
  server.add_routing_middleware();

  server.on_get("/page", [&data](const http::request &req) {
    auto page = http::template_engine::render(index_html, data);
    return http::response::ok(page, http::mime_types::TYPE_TEXT_HTML);
  });

  server.run();

  return 0;
}

