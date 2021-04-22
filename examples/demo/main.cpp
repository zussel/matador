#include "matador/logger/log_manager.hpp"

#include "matador/utils/file.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"
#include "matador/http/middleware.hpp"

using namespace matador;

class authentication_middleware : public http::middleware
{
public:
  http::response process(http::request &req, const next_func_t &next) override
  {
    return http::response::redirect("secret");
    return next();
  }
};

int main(int /*argc*/, char* /*argv*/[])
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  http::server server(9082);
  server.add_routing_middleware();
  server.add_middleware(std::make_shared<authentication_middleware>());

  server.on_get("/index", [](const http::request &) {
    file f("html/login.html", "r");

    if (!f.is_open()) {
      return http::response::not_found();
    }

    auto content = read_as_text(f);

    return http::response::ok(content, http::mime_types::TYPE_TEXT_HTML);
  });

  server.on_get("/secret", [](const http::request &) {
    file f("html/secret.html", "r");

    if (!f.is_open()) {
      return http::response::not_found();
    }

    auto content = read_as_text(f);

    return http::response::ok(content, http::mime_types::TYPE_TEXT_HTML);
  });

  server.on_post("/login", [](const http::request &req) {

    auto username = req.form_data().at("username");
    auto password = req.form_data().at("password");

    return http::response::not_found();
  });

  server.run();

  return 0;
}
