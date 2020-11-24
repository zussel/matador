#include "matador/http/http_server.hpp"

using namespace matador;

int main(int /*argc*/, char* /*argv*/[])
{
  http::server server(8081);

  server.run();

  return 0;
}

