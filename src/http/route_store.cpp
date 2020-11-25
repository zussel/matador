#include "matador/http/route_store.hpp"

#include "matador/utils/stream.hpp"
#include "matador/utils/string.hpp"

#include <utility>
#include <vector>

namespace matador {
namespace http {


route_store::route_store(std::string context_root)
  : context_root_(std::move(context_root))
{
  route_tree_.insert(route_tree_.begin(), route_endpoint(context_root_, context_root_, http::http::UNKNOWN));
}

void route_store::add(const std::string &path, http::http::method_t method, route_endpoint::t_request_handler request_handler)
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

route_endpoint route_store::find(const std::string &path, http::http::method_t method)
{
  auto i = find_interal(path, method);
  if (i == route_tree_.end()) {
    return route_endpoint("", path, http::http::UNKNOWN);
  }
  return *i;
}

bool route_store::contains(const std::string &path, http::http::method_t method)
{
  return find_interal(path, method) == route_tree_.end();
}

void route_store::dump(std::ostream &out)
{
  make_stream(route_tree_).filter([](const route_endpoint &ep) {
    return ep.method() != http::http::UNKNOWN;
  }).for_each([&out](const route_endpoint &ep) {
    out << "endpoint: " << ep.endpoint_name() << " (path: " << ep.endpoint_path() << ", method: " << http::http::to_string(ep.method()) << ")\n";
  });
}

tree<route_endpoint>::iterator route_store::find_interal(const std::string &path, http::http::method_t method)
{
  // split path into segments
  std::vector<std::string> route_path_elements;
  split(path, '/', route_path_elements);

  // if no segments return
  if (route_path_elements.empty()) {
    return route_tree_.end();
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
}
}
