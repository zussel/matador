#include <type_traits>
#include <iostream>
#include <utility>

#include "matador/utils/string.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/route_endpoint.hpp"
#include "matador/http/route_store.hpp"

using namespace matador::http;
using namespace std::placeholders;

class auth_service
{
public:
  explicit auth_service(server &s) : server_(s) {
    s.on_post("/v1/auth/login", [this](const request &req) { return login(req); });
    s.on_post("/v1/auth/logout", [this](const request &req) { return login(req); });
  }

  response login(const request &req)
  {
    // extract username and password from headers
    req.headers.at("Authentication");

    return response();
  }

  response logout(const request &request)
  {
    return response();
  }

  void login(const std::string username, const std::string &passwd)
  {
  }

private:
  server& server_;
};


int main(int /*argc*/, char* /*argv*/[])
{

  server s(7091, "api");

  auth_service auth(s);
  
//  route_store rs("api");
//
//  rs.add("/v1/auth/login", http::http::POST);
//  rs.add("/v1/auth/logout", http::http::POST);
//  rs.add("/v1/user/create", http::http::POST);
//  rs.add("/v1/user", http::http::GET);
//  rs.add("/v1/user/edit", http::http::PUT);
//  rs.add("/v1/user/delete", http::http::DELETE);
//  rs.add("/v1/role/create", http::http::POST);
//  rs.add("/v1/role", http::http::GET);
//  rs.add("/v1/role/edit", http::http::PUT);
//  rs.add("/v1/role/delete", http::http::DELETE);

//  std::vector<std::string> strvec = {"eins", "zwei", "drei", "vier", "fünf", "sechs", "sieben"};

//  make_stream(strvec).filter([](const std::string &val) {
//    return val[0] == 's';
//  }).for_each([](const std::string &val) {
//    std::cout << "value: " << val << "\n";
//  });

//  rs.dump(std::cout);

//  auto ret = rs.find("/v1/user/edit", http::PUT);

//  std::cout << "found endpoint: " << ret.endpoint_name() << " (path: " << ret.endpoint_path() << ", method: " << http::http::to_string(ret.method()) << ")\n";
}

//  http::server serv;
//
//  serv.on_get("/", [](http::request &request) {
//    return http::response;
//  });
//
//  serv.listen(7090);

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
