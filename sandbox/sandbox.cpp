#include <type_traits>
#include <iostream>
#include <utility>

#include "matador/logger/log_manager.hpp"

#include "matador/utils/string.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/route_path.hpp"
#include "matador/http/routing_engine.hpp"

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

class auth_service
{
public:
  explicit auth_service(server &s) : server_(s) {
    s.on_post("/v1/auth/login", [this](const request &req, const route_path::t_path_param_map &path_params) { return login(req, path_params); });
    s.on_post("/v1/auth/logout", [this](const request &req, const route_path::t_path_param_map &path_params) { return login(req, path_params); });
  }

  response login(const request &req, const route_path::t_path_param_map &)
  {
    //auto credentials = json_to_object<credential>(req.body);
    // extract username and password from headers
    req.headers.at("Authentication");

    response resp;
    return response();
  }

  response logout(const request &request, const route_path::t_path_param_map &path_params)
  {
    return response();
  }

  void login(const std::string username, const std::string &passwd)
  {
  }

private:
  server& server_;
};


class application
{
public:
  explicit application(unsigned short port)
    : server_(port)
    , auth_(server_)
  {}

  void run()
  {
    server_.run();
  }

private:
  server server_;
  auth_service auth_;
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
