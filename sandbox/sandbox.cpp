#include "matador/logger/log_manager.hpp"

#include "matador/http/static_file_service.hpp"
#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/route_path.hpp"
#include "matador/http/url.hpp"

#include "matador/utils/time.hpp"

using namespace matador::http;
using namespace std::placeholders;

class response_builder
{
public:

  response_builder& ok() {

    return *this;
  }
  response_builder& status(http::status_t stat);
  response_builder& body(const std::string &b);
  response_builder& content_type(const std::string &ct);
  response_builder& accept_encoding(const std::string ae);
  response_builder& add_header(const std::string &key, const std::string &value);
  response build();

  void reset()
  {
    response_ = response();
    response_.version.major = 1;
    response_.version.minor = 1;
  }
private:
  response response_;
};

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

class auth_service
{
public:
  explicit auth_service(server &s)
    : server_(s)
    , log_(matador::create_logger("AuthService"))
  {
    s.on_post("/api/v1/auth/login", [this](const request &req, const t_path_param_map &path_params) { return login(req, path_params); });
    s.on_post("/api/v1/auth/logout", [this](const request &req, const t_path_param_map &path_params) { return logout(req, path_params); });
  }

  response login(const request &req, const t_path_param_map &)
  {
    log_.info("login");

    //req.body
    //auto credentials = json_to_object<credential>(req.body);
    // extract username and password from headers
//    req.headers.at("Authentication");

    user u { 0, "herb", "herb123", "Herbert",  "Grönemeyer"};

    return response::json(u);
  }

  response logout(const request &request, const t_path_param_map &path_params)
  {
    log_.info("logout");
    return response();
  }

  void login(const std::string username, const std::string &passwd)
  {
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
//  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  // creates a web application at port 7091
  application app(7091);

  app.run();
}
