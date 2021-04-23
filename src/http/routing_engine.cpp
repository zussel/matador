#include "matador/http/routing_engine.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"

#include "matador/utils/stream.hpp"
#include "matador/utils/string.hpp"

#include <regex>

namespace matador {
namespace http {

bool prepare_route_path_elements(const std::string &path, std::list<std::string> &rpe);

// ^(\/(([\w]+)|(\{[\w\: \\\+]+\}))*)(\/(([\w]+)|(\{[\w \\\:\+]+\}))+)*\/?

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

routing_engine::const_iterator
routing_engine::match(request &req) const
{
  auto it = std::find_if(routes_.begin(), routes_.end(), [&req](const route_endpoint_ptr &ep) {
    return ep->match(req);
  });
  return it;
}

void routing_engine::dump(std::ostream &out)
{
  make_stream(routes_).filter([](const auto &ep) {
    return ep->method() != http::http::UNKNOWN;
  }).for_each([&out](const route_endpoint_ptr &ep) {
    out << "endpoint: " << ep->path_spec() << " (method: " << http::http::to_string(ep->method()) << ", regex: " << ep->path_regex() << ")\n";
  });
}

bool routing_engine::valid(const routing_engine::const_iterator& it) const
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

  return it;
}

routing_engine::route_endpoint_ptr create_route_endpoint(
  const std::string &path_spec, http::method_t method,
  const t_request_handler &request_handler
)
{
  static std::regex route_regex { R"((\*\.\*)|\{(\w+)(:\s*(.*))?\}|([a-zA-Z0-9-_]+))" };

  t_size_string_map path_param_to_index_map;

  // check for root path explicitly
  if (path_spec == "/") {
    return std::make_shared<route_endpoint>(path_spec, R"(\/)", method, request_handler, path_param_to_index_map);
  }

  std::smatch what;

  std::list<std::string> parts;

  prepare_route_path_elements(path_spec, parts);

  std::string result_regex;

  size_t index = 0;
  for (const auto &part : parts) {
    if (part.empty()) {
      continue;
    }

    if (!std::regex_match(part, what, route_regex)) {
      throw std::logic_error("invalid route spec part: " + part);
    }

    result_regex += R"(\/)";
    if (what[1].matched) {
      // static files
      result_regex += R"(([^?]*))";
    } else if (what[5].matched) {
      // plain path element
      result_regex += what[5].str();
    } else if (what[4].matched) {
      // path param with regex
      result_regex += "(" + what[4].str() + ")";
      // what[2] is path name
      path_param_to_index_map.insert(std::make_pair(++index, what[2].str()));
    } else if (what[2].matched) {
      // path param
      result_regex += R"((\w+))";
      path_param_to_index_map.insert(std::make_pair(++index, what[2].str()));
    } else {
      throw std::logic_error("invalid route spec pattern match");
    }
  }

  return std::make_shared<route_endpoint>(path_spec, result_regex, method, request_handler, path_param_to_index_map);
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
