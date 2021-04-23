#include "matador/http/route_endpoint.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"

#include <utility>
#include <regex>

namespace matador {
namespace http {

route_endpoint::route_endpoint(std::string path_spec, const std::string& path_regex, http::method_t method, t_request_handler request_handler, t_size_string_map param_index_map)
  : path_spec_(std::move(path_spec))
  , path_regex_(path_regex)
  , path_regex_str_(path_regex)
  , method_(method)
  , request_handler_(std::move(request_handler))
  , param_index_map_(std::move(param_index_map))
{
}

bool route_endpoint::match(request &req)
{
  if (method_ != req.method()) {
    return false;
  }
  std::smatch what;
  std::string path = req.url();
  if (!std::regex_match(path, what, path_regex_)) {
    return false;
  }

  for (size_t i = 1; i < what.size(); ++i) {
    auto it = param_index_map_.find(i);
    if (it != param_index_map_.end()) {
      req.path_params_.insert(std::make_pair(it->second, what[i].str()));
    }
  }

  return true;
}

http::method_t route_endpoint::method() const
{
  return method_;
}

const std::string &route_endpoint::path_spec() const
{
  return path_spec_;
}

const std::string &route_endpoint::path_regex() const
{
  return path_regex_str_;
}

const t_size_string_map& route_endpoint::param_map() const
{
  return param_index_map_;
}

response route_endpoint::execute(const request &req)
{
  return request_handler_(req);
}

}
}
