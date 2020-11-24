#include <type_traits>
#include <iostream>
#include <utility>

#include "matador/utils/tree.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/stream.hpp"

#include "matador/http/route_endpoint.hpp"

using namespace matador;

class route_store
{
public:
  explicit route_store(std::string context_root)
    : context_root_(std::move(context_root))
  {
    route_tree_.insert(route_tree_.begin(), http::route_endpoint(context_root_, context_root_, http::http::UNKNOWN));
  }

  void add(const std::string& path, http::http::method_t method)
  {
    // split path into segments
    std::vector<std::string> route_path_elements;
    split(path, '/', route_path_elements);

    // if no segments return
    if (route_path_elements.empty()) {
      return;
    }

    // if first segment is empty set its name to "root"
    if (route_path_elements.at(0).empty()) {
      route_path_elements[0] = context_root_;
    }

    //
    auto parent = route_tree_.begin();
    bool first = true;
    for (const auto &elem : route_path_elements) {
      if (first) {
        if (parent->endpoint_name() != elem) {
          // invalid parent node
          return;
        }
        first = false;
      } else {
        auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent),
                                [&elem](const http::route_endpoint &route) {
                                  return route.endpoint_name() == elem;
                                });
        if (rit == route_tree_.end(parent)) {
          // unknown element, create new
          parent = route_tree_.push_back_child(parent, http::route_endpoint(elem, "", http::http::UNKNOWN));
        } else {
          // element exists already
          parent = rit;
        }
      }
    }
    parent->method(method);
    parent->endpoint_path(path);
  }

  http::route_endpoint find(const std::string &path)
  {
    // split path into segments
    std::vector<std::string> route_path_elements;
    split(path, '/', route_path_elements);

    // if no segments return
    if (route_path_elements.empty()) {
      return http::route_endpoint("", path, http::http::UNKNOWN);
    }

    // if first segment is empty set its name to "root"
    if (route_path_elements.at(0).empty()) {
      route_path_elements[0] = context_root_;
    }

    auto parent = route_tree_.begin();
    bool first = true;
    for (const auto &elem : route_path_elements) {
      if (first) {
        if (parent->endpoint_name() != elem) {
          // invalid parent node
          return http::route_endpoint("", path, http::http::UNKNOWN);
        }
        first = false;
      } else {
        auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent),
                                [&elem](const http::route_endpoint &route) {
                                  return route.endpoint_name() == elem;
                                });
        if (rit == route_tree_.end(parent)) {
          // unknown element, create new
          return http::route_endpoint("", path, http::http::UNKNOWN);
        } else {
          // element exists already
          parent = rit;
        }
      }
    }

    return *parent;
  }

  void dump(std::ostream &out)
  {
    make_stream(route_tree_).filter([](const http::route_endpoint &ep) {
      return ep.method() != http::http::UNKNOWN;
    }).for_each([&out](const http::route_endpoint &ep) {
      out << "endpoint: " << ep.endpoint_name() << " (path: " << ep.endpoint_path() << ", method: " << http::http::to_string(ep.method()) << ")\n";
    });
  }

private:
  std::string context_root_;
  tree<http::route_endpoint> route_tree_;
};

int main(int /*argc*/, char* /*argv*/[])
{

  route_store rs("api");

  rs.add("/v1/auth/login", http::http::POST);
  rs.add("/v1/auth/logout", http::http::POST);
  rs.add("/v1/user/create", http::http::POST);
  rs.add("/v1/user", http::http::GET);
  rs.add("/v1/user/edit", http::http::PUT);
  rs.add("/v1/user/delete", http::http::DELETE);
  rs.add("/v1/role/create", http::http::POST);
  rs.add("/v1/role", http::http::GET);
  rs.add("/v1/role/edit", http::http::PUT);
  rs.add("/v1/role/delete", http::http::DELETE);

//  rs.dump(std::cout);

  auto ret = rs.find("/v1/user/edit");

  std::cout << "found endpoint: " << ret.endpoint_name() << " (path: " << ret.endpoint_path() << ", method: " << http::http::to_string(ret.method()) << ")\n";
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
