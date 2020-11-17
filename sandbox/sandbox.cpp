#include <type_traits>
#include <iostream>

#include "matador/net/io_service.hpp"

using namespace matador;

struct http_server
{
  http_server(unsigned port);

  template < typename CB >
  void get(const std::string &route, CB callback);

  void run();

  io_service service_;
};

int main(int /*argc*/, char* /*argv*/[])
{

  http_server server(8081);

  return 0;
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
