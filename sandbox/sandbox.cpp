#include "matador/logger/log_manager.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/route_path.hpp"

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
    s.on_post("/api/v1/auth/login", [this](const request &req, const route_path::t_path_param_map &path_params) { return login(req, path_params); });
    s.on_post("/api/v1/auth/logout", [this](const request &req, const route_path::t_path_param_map &path_params) { return logout(req, path_params); });
  }

  response login(const request &req, const route_path::t_path_param_map &)
  {
    log_.info("login");

    req.body
    //auto credentials = json_to_object<credential>(req.body);
    // extract username and password from headers
//    req.headers.at("Authentication");

    user u { 0, "herb", "herb123", "Herbert",  "Grönemeyer"};

    return response::json(u);
  }

  response logout(const request &request, const route_path::t_path_param_map &path_params)
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

class app_service
{
public:
  explicit app_service(server &s)
    : server_(s)
    , log_(matador::create_logger("AppService"))
  {
    s.on_get("/app/*.*", [this](const request &req, const route_path::t_path_param_map&) { return serve(req); });
  }

private:
  response serve(const request &req)
  {
    log_.info("serving file %s", req.url.c_str());

    matador::file f("." + req.url, "r");

    if (!f.is_open()) {

    }

    // obtain file size:
    fseek (f.stream() , 0 , SEEK_END);
    size_t size = ftell (f.stream());
    rewind (f.stream());

    response resp;
    resp.body.resize(size);

    fread(const_cast<char*>(resp.body.data()), 1, size, f.stream());

    f.close();

    resp.status = http::OK;

    resp.content_type.type = mime_types::TEXT_HTML;
    resp.content_type.length = size;

    resp.version.major = 1;
    resp.version.minor = 1;

    resp.headers.insert(std::make_pair(response_header::DATE, to_string(matador::time::now(), "%a, %d %b %Y %H:%M:%S %Z")));
    resp.headers.insert(std::make_pair(response_header::SERVER, "Matador/0.7.0"));
    resp.headers.insert(std::make_pair(response_header::CONNECTION, "Closed"));

    return resp;
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
    , app_(server_)
  {}

  void run()
  {
    server_.run();
  }

private:
  server server_;
  auth_service auth_;
  app_service app_;
};


int main(int /*argc*/, char* /*argv*/[])
{
//  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  // creates a web application at port 7091
  application app(7091);

  app.run();
}

// server
  /*
  GET / HTTP/1.1
  Host: localhost:7090
  User-Agent: curl/7.70.0
  Accept: * / *
  */

// client
//(HTTP/1.1 200 OK
//Server: Matador/0.7.0
//Content-Length: 111
//Content-Language: de
//Connection: close
//Content-Type: text/html
//
//<!doctype html>
//<html>
//  <head>
//    <title>Dummy!</title>
//  </head>
//  <body>
//    <p>Help!</p>
//  </body>
//</html>
