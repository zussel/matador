#include "matador/http/route_engine.hpp"

#include "matador/utils/stream.hpp"
#include "matador/utils/string.hpp"

#include <utility>
#include <vector>
#include <regex>

namespace matador {
namespace http {

bool prepare_route_path_elements(const std::string &path, std::list<std::string> &rpe);

void route_engine::add(const std::string &path, http::http::method_t method, route_endpoint::t_request_handler request_handler)
{
  // split path into segments
  std::list<std::string> route_path_elements;
  if (!prepare_route_path_elements(path, route_path_elements)) {
    return;
  }

  auto parent = route_tree_.begin();
  bool first = true;
  for (const auto &elem : route_path_elements) {
    if (first) {
      auto rit = std::find_if(route_tree_.begin(), route_tree_.end(),
                              [&elem](const route_endpoint &route) {
                                return route.endpoint_name() == elem;
                              });
      if (rit == route_tree_.end()) {
        // unknown element, create new
        parent = route_tree_.insert(parent, route_endpoint(elem, path, http::http::UNKNOWN));
      } else {
        // element exists already
        parent = rit;
      }

      first = false;
    } else {
      auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent),
                              [&elem](const route_endpoint &route) {
                                return route.endpoint_name() == elem;
                              });
      if (rit == route_tree_.end(parent)) {
        // unknown element, create new
        parent = route_tree_.push_back_child(parent, route_endpoint(elem, "", http::http::UNKNOWN));
      } else {
        // element exists already
        parent = rit;
      }
    }
  }
  parent->method(method);
  parent->endpoint_path(path);
  parent->request_handler(std::move(request_handler));
}

route_endpoint route_engine::find(const std::string &path, http::http::method_t method)
{
  auto i = find_internal(path, method);
  if (i == route_tree_.end()) {
    return route_endpoint("", path, http::http::UNKNOWN);
  }
  return *i;
}

bool route_engine::contains(const std::string &path, http::http::method_t method)
{
  return find_internal(path, method) == route_tree_.end();
}

void route_engine::dump(std::ostream &out)
{
  make_stream(route_tree_).filter([](const route_endpoint &ep) {
    return ep.method() != http::http::UNKNOWN;
  }).for_each([&out](const route_endpoint &ep) {
    out << "endpoint: " << ep.endpoint_name() << " (path: " << ep.endpoint_path() << ", method: " << http::http::to_string(ep.method()) << ")\n";
  });
}

tree<route_endpoint>::iterator route_engine::find_internal(const std::string &path, http::http::method_t method)
{
  // split path into segments
  std::list<std::string> route_path_elements;
  if (!prepare_route_path_elements(path, route_path_elements)) {
    return route_tree_.end();
  }

  auto parent = route_tree_.begin();
  bool first = true;
  for (const auto &elem : route_path_elements) {
    if (first) {
      if (parent->endpoint_name() != elem) {
        // invalid parent node
        return route_tree_.end();
      }
      first = false;
    } else {
      auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent),
                              [&elem](const route_endpoint &route) {
                                return route.endpoint_name() == elem;
                              });
      if (rit == route_tree_.end(parent)) {
        // unknown element, create new
        return route_tree_.end();
      } else {
        // element exists already
        parent = rit;
      }
    }
  }

  if (parent->method() != method) {
    return route_tree_.end();
  }
  return parent;
}

bool prepare_route_path_elements(const std::string &path, std::list<std::string> &rpe)
{
  split(path, '/', rpe);

  // if no segments return
  if (rpe.empty()) {
    return false;
  }

  // if first segment is empty set its name to "root"
  if (rpe.front().empty()) {
    rpe.pop_front();
  }
  return true;
}

bool is_path_param(const std::string &elem)
{
  std::regex re(R"(\{(\w+)(:\s*(.*))?\}|([a-zA-Z0-9-_]+))");

  std::smatch what;

  if (!std::regex_match(elem, what, re)) {
    return false;
  } else {
    return true;
  }
}

}
}
