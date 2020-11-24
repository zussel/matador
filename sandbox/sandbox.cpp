#include <type_traits>
#include <iostream>

#include "matador/utils/tree.hpp"
#include "matador/utils/string.hpp"

#include "matador/http/route_endpoint.hpp"

using namespace matador;

class route_store
{
public:
  route_store() = default;

  void add(const std::string& path)
  {
    std::vector<std::string> route_path_elements;
    split(path, '/', route_path_elements);

    if (route_path_elements.empty()) {
      return;
    }
    if (route_path_elements.at(0).empty()) {
      route_path_elements[0] = "root";
    }

    auto parent = route_tree_.begin();
    for (const auto &elem : route_path_elements) {
      auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent), [&elem](const http::route_endpoint &route) {
        return route.endpoint_name() == elem;
      });
      if (rit == route_tree_.end(parent)) {

      } else {

      }
    }
  }

  http::route_endpoint find(const std::string &route)
  {
    return http::route_endpoint();
  }

private:
  tree<http::route_endpoint> route_tree_;
};

int main(int /*argc*/, char* /*argv*/[])
{

  route_store rs;

  rs.add("/login");

//  http::route_endpoint rr(root, http::http::GET);
//
//  std::vector<std::string> route_path_elements;
//
//  split(root, '/', route_path_elements);
//  split(login, '/', route_path_elements);
//  split(person, '/', route_path_elements);

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
