#include "main_page.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/template_engine.hpp"

#include "matador/logger/log_manager.hpp"

using namespace matador;
using namespace matador::http;

main_page::main_page(matador::http::server &s, matador::persistence &p)
  : log_(matador::create_logger("MainPage"))
  , persistence_(p)
{
  file f("../templates/main.matador", "r");
  auto tmpl = read_as_text(f);
  f.close();

  index_template_ = template_engine::build(tmpl);

  s.on_get("/", [this](const request &req) {
    return view(req);
  });
}

matador::http::response main_page::view(const matador::http::request &p)
{
  json data;
  data["title"] = "Movies";

  return response::ok(template_engine::render(index_template_, data), mime_types::TYPE_TEXT_HTML);
}
