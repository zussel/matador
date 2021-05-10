#include "services/auth_service.hpp"
#include "pages/main_page.hpp"

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

  http::response process(http::request &, const next_func_t &next) override
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

  net::init();

  http::server server(19082);
  server.add_routing_middleware();
  server.add_middleware(std::make_shared<authentication_middleware>());

  os::chdir("web");

  http::serve_static_files_at("/css/*.*", server);
  http::serve_static_files_at("/js/*.*", server);

  auth_service auth(server);
  main_page mpage(server);

  server.run();

  net::cleanup();

  return 0;
}
