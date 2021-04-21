#include "matador/logger/log_manager.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/response.hpp"

using namespace matador;

int main(int /*argc*/, char* /*argv*/[])
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  http::server server(8081);
  server.add_routing_middleware();

  server.on_get("/api/user", [](const http::request &req) {
    return http::response::ok("hello world", http::mime_types::TYPE_TEXT_PLAIN);
  });
  server.run();

  return 0;
}

