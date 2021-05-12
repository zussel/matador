#include "main_page.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/template_engine.hpp"

#include "matador/logger/log_manager.hpp"

using namespace matador;
using namespace matador::http;

main_page::main_page(matador::http::server &server, matador::persistence &p)
  : log_(matador::create_logger("MainPage"))
  , persistence_(p)
{
  file f("../templates/movies.matador", "r");
  auto tmpl = read_as_text(f);
  f.close();

  index_template_ = template_engine::build(tmpl);

  server.on_get("/", [this](const request &req) {
    return get_index(req);
  });

  server.on_get("/login", [](const request &) {
    return response::from_file("login.html");
  });

  server.on_get("/secure", [](const request &) {
    return response::from_file("secure.html");
  });
}

matador::http::response main_page::get_index(const request &p)
{
  json data;
  data["title"] = "Movies";
  data["movies"] = json::array();

  return response::ok(template_engine::render(index_template_, data), mime_types::TYPE_TEXT_HTML);
}
