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
  : route_regex_(R"((\*\.\*)|\{(\w+)(:\s*(.*))?\}|([a-zA-Z0-9-_]+))")
{}

void routing_engine::add(const std::string &path, http::http::method_t method, const t_request_handler& request_handler)
{
  auto it = find_internal(path, method);

  if (it != routes_.end()) {
    return;
  }

  auto endpoint = create_route_endpoint(path, method, request_handler);

  routes_.push_back(endpoint);
}

routing_engine::iterator routing_engine::find(const std::string &path, http::http::method_t method)
{
  return find_internal(path, method);
}

routing_engine::iterator
routing_engine::match(const std::string &path, http::http::method_t method, t_path_param_map &path_params)
{
  auto it = std::find_if(routes_.begin(), routes_.end(), [method, &path, &path_params](const route_endpoint_ptr &ep) {
    return ep->match(path, method, path_params);
  });
  return it;
}

void routing_engine::dump(std::ostream &out)
{
  for (const auto &it : routes_) {
    //out << "endpoint: " << it->endpoint_name() << " (path: " << it->endpoint_path() << ", method: " << http::http::to_string(it->method()) << ")\n";
  }

//  make_stream(route_tree_).filter([](const route_path_ptr &ep) {
//    return ep->method() != http::http::UNKNOWN;
//  }).for_each([&out](const route_path_ptr &ep) {
//    out << "endpoint: " << ep->endpoint_name() << " (path: " << ep->endpoint_path() << ", method: " << http::http::to_string(ep->method()) << ")\n";
//  });
}

bool routing_engine::valid(const routing_engine::iterator& it) const
{
  return it != routes_.end();
}

routing_engine::iterator routing_engine::find_internal(
  const std::string &path,
  http::http::method_t method)
{
  auto it = std::find_if(routes_.begin(), routes_.end(), [method, &path](const route_endpoint_ptr &ep) {
    return ep->method() == method && ep->path_spec() == path;
  });
}

routing_engine::route_endpoint_ptr routing_engine::create_route_endpoint(
  const std::string &path_spec, http::method_t method,
  const t_request_handler &request_handler
)
{
  std::smatch what;

  std::unordered_map<std::string, size_t> path_param_to_index_map;
  std::list<std::string> parts;

  prepare_route_path_elements(path_spec, parts);

  std::string result_regex;

  size_t index = 0;
  for (const auto &part : parts) {
    if (part.empty()) {
      continue;
    }

    if (!std::regex_match(part, what, route_regex_)) {
      throw std::logic_error("invalid route spec part: " + part);
    }

    result_regex += R"(\/)";
    if (what[1].matched) {
      // static files
      result_regex += "(.*)";
    } else if (what[5].matched) {
      // plain path element
      result_regex += what[5].str();
    } else if (what[4].matched) {
      // path param with regex
      result_regex += "(" + what[4].str() + ")";
      // what[2] is path name
      path_param_to_index_map.insert(std::make_pair(what[2].str(), index++));
    } else if (what[2].matched) {
      // path param
      result_regex += R"((\w+))";
      path_param_to_index_map.insert(std::make_pair(what[2].str(), index++));
    } else {
      throw std::logic_error("invalid route spec pattern match");
    }
  }

  return std::make_shared<route_endpoint>(path_spec, std::regex(result_regex), method, request_handler);
}

bool prepare_route_path_elements(const std::string &path, std::list<std::string> &rpe)
{
  split(path, '/', rpe);

  // if no segments return
  if (rpe.empty()) {
    return false;
  }
  if (rpe.front().empty()) {
    rpe.pop_front();
  }
  return true;
}

}
}
