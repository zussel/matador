#include "matador/http/routing_engine.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"

#include "matador/utils/stream.hpp"
#include "matador/utils/string.hpp"

#include <regex>

namespace matador {
namespace http {

bool prepare_route_path_elements(const std::string &path, std::list<std::string> &rpe);

routing_engine::routing_engine()
  : route_regex_(R"(\{(\w+)(:\s*(.*))?\}|([a-zA-Z0-9-_]+))")
{}

void routing_engine::add(const std::string &path, http::http::method_t method, const route_path::t_request_handler& request_handler)
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
                              [&elem](const route_path_ptr &route) {
                                return route->endpoint_name() == elem;
                              });
      if (rit == route_tree_.end()) {
        // unknown element, create new
        parent = route_tree_.insert(parent, std::make_shared<route_path>(elem, path, http::http::UNKNOWN));
      } else {
        // element exists already
        parent = rit;
      }

      first = false;
    } else {
      auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent),
                              [&elem](const route_path_ptr &route) {
                                return route->endpoint_name() == elem;
                              });
      if (rit == route_tree_.end(parent)) {
        // unknown element, create new
        parent = route_tree_.push_back_child(parent, make_route_path(elem, method, path, [](const request&, const route_path::t_path_param_map&) { return response(); }));
      } else {
        // element exists already
        parent = rit;
      }
    }
  }

  *parent = make_route_path((*parent)->endpoint_name(), method, path, request_handler);
}

routing_engine::iterator routing_engine::find(const std::string &path, http::http::method_t method, route_path::t_path_param_map &path_params)
{
  path_params.clear();
  return find_internal(path, method, path_params);
}

void routing_engine::dump(std::ostream &out)
{
  for (const auto &it : route_tree_) {
    out << "endpoint: " << it->endpoint_name() << " (path: " << it->endpoint_path() << ", method: " << http::http::to_string(it->method()) << ")\n";
  }

//  make_stream(route_tree_).filter([](const route_path_ptr &ep) {
//    return ep->method() != http::http::UNKNOWN;
//  }).for_each([&out](const route_path_ptr &ep) {
//    out << "endpoint: " << ep->endpoint_name() << " (path: " << ep->endpoint_path() << ", method: " << http::http::to_string(ep->method()) << ")\n";
//  });
}

bool routing_engine::valid(const routing_engine::iterator& it) const
{
  return it != route_tree_.end();
}

routing_engine::iterator routing_engine::find_internal(
  const std::string &path,
  http::http::method_t method,
  route_path::t_path_param_map &path_params
)
{
  if (route_tree_.empty()) {
    return route_tree_.end();
  }

  // split path into segments
  std::list<std::string> route_path_elements;
  if (!prepare_route_path_elements(path, route_path_elements)) {
    return route_tree_.end();
  }

  auto parent = route_tree_.begin();
  bool first = true;
  for (const auto &elem : route_path_elements) {
    if (first) {
      if ((*parent)->endpoint_name() != elem) {
        // invalid parent node
        return route_tree_.end();
      }
      first = false;
    } else {
      auto rit = std::find_if(route_tree_.begin(parent), route_tree_.end(parent),
                              [&elem, &path_params](const route_path_ptr &route) {
                                return route->match(elem, path_params);
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

  if ((*parent)->method() != method) {
    return route_tree_.end();
  }
  return parent;
}

routing_engine::route_path_ptr
routing_engine::make_route_path(const std::string &name, http::method_t method, const std::string &path,
                                const route_path::t_request_handler &request_handler)
{
  std::smatch what;

  if (!std::regex_match(name, what, route_regex_)) {
    throw std::logic_error("invalid route path: " + name);
  } else {
    // group 1 -> path param
    // group 1 + 2 -> path param restricted with regex
    // group 4 -> plain path element
    if (what[4].matched) {

      return std::make_shared<route_path>(name, path, method, request_handler);
    } else if (what[3].matched) {
      return std::make_shared<regex_path_param_route_path>(
        what[3].str(), what[1].str(), name, path, method, request_handler
      );
    } else if (what[1].matched) {
      return std::make_shared<path_param_route_path>(what[1].str(), name, path, method, request_handler);
    } else {
      throw std::logic_error("invalid route path: " + name);
    }
  }
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

}
}
