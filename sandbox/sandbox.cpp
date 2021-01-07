#include "matador/logger/log_manager.hpp"

#include "matador/utils/json_mapper.hpp"

#include "matador/http/static_file_service.hpp"
#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/url.hpp"

/*
 * curl -v -H "Content-Type: application/json" --data '{"username":"otto","password":"otto123"}' -X POST http://localhost:7091/api/v1/auth/login?query=9
 */

using namespace matador::http;
using namespace std::placeholders;

struct user
{
  long id;
  std::string username;
  std::string password;
  std::string first_name;
  std::string last_name;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("username", username);
    serializer.serialize("password", password);
    serializer.serialize("first_name", first_name);
    serializer.serialize("last_name", last_name);
  }
};

struct credential
{
  std::string username;
  std::string password;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("username", username);
    serializer.serialize("password", password);
  }
};

class auth_service
{
public:
  explicit auth_service(server &s)
    : server_(s)
    , log_(matador::create_logger("AuthService"))
  {
    s.on_post("/api/v1/auth/login", [this](const request &req) { return login(req); });
    s.on_post("/api/v1/auth/logout", [this](const request &req) { return logout(req); });
  }

  response login(const request &req)
  {
    log_.info("login");

    matador::json_mapper<credential> mapper;

    auto credentials = mapper.object_from_string(req.body().c_str());

    log_.info("user %s logging in", credentials.username.c_str());

    // extract username and password from headers
//    req.headers.at("Authentication");

    user u { 0, "herb", "herb123", "Herbert",  "Grönemeyer"};

    return response::json(u);
  }

  response logout(const request &)
  {
    log_.info("logout");
    return response::no_content();
  }

private:
  server& server_;

  matador::logger log_;
};

class application
{
public:
  explicit application(unsigned short port)
    : server_(port)
    , auth_(server_)
    , app_("/app/*.*", server_)
  {}

  void run()
  {
    server_.run();
  }

private:
  server server_;
  auth_service auth_;
  static_file_service app_;
};


int main(int /*argc*/, char* /*argv*/[])
{
  matador::add_log_sink(matador::create_stdout_sink());

  // creates a web application at port 7091
  application app(7091);

  app.run();
}
