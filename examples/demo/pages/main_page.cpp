#include "main_page.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"

#include "matador/logger/log_manager.hpp"

using namespace matador::http;

main_page::main_page(matador::http::server &server)
  : log_(matador::create_logger("MainPage"))
{
  server.on_get("/", [](const request &) {
    return response::from_file("index.html");
  });

  server.on_get("/login", [](const request &) {
    return response::from_file("login.html");
  });

  server.on_get("/secure", [](const request &) {
    return response::from_file("secure.html");
  });
}
