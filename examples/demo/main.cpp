#include "matador/logger/log_manager.hpp"

#include "matador/utils/file.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"
#include "matador/http/middleware.hpp"
#include "matador/http/static_file_service.hpp"

using namespace matador;

class authentication_middleware : public http::middleware
{
public:
  authentication_middleware()
  {
    usermap_.insert(std::make_pair("hans", "hans123"));
    usermap_.insert(std::make_pair("otto", "otto123"));
  }

  http::response process(http::request &req, const next_func_t &next) override
  {
    //return http::response::redirect("index");
    return next();
  }

private:
  std::map<std::string, std::string> usermap_;
};

int main(int /*argc*/, char* /*argv*/[])
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  http::server server(9082);
  server.add_routing_middleware();
  server.add_middleware(std::make_shared<authentication_middleware>());

  http::static_file_service css_files_("/html/css/*.*", server);

  server.on_get("/", [](const http::request &) {
    return http::response::from_file("html/index.html");
  });

  server.on_get("/login", [](const http::request &) {
    return http::response::from_file("html/login.html");
  });

  server.on_get("/secret", [](const http::request &) {
    return http::response::from_file("html/secret.html");
  });

  server.on_post("/login", [](const http::request &req) {

    auto username = req.form_data().at("uname");
    auto password = req.form_data().at("pwd");

    return http::response::redirect("secret");
  });

  server.run();

  return 0;
}
