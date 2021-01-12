#include "matador/http/request.hpp"
#include "matador/http/request_header.hpp"

namespace matador {
namespace http {

request::request(http::method_t method, std::string host, std::string url)
  : method_(method)
  , url_(std::move(url))
  , host_(std::move(host))
{
  headers_.insert(std::make_pair(request_header::HOST, host));
}

http::method_t request::method() const
{
  return method_;
}

std::string request::url() const
{
  return url_;
}

std::string request::query() const
{
  return query_;
}

std::string request::fragment() const
{
  return fragment_;
}

http::version request::version() const
{
  return version_;
}

std::string request::host() const
{
  return host_;
}

const http::content& request::content() const
{
  return content_;
}

const t_string_param_map& request::headers() const
{
  return headers_;
}

const t_string_param_map& request::path_params() const
{
  return path_params_;
}

const t_string_param_map& request::query_params() const
{
  return query_params_;
}

const std::string& request::body() const
{
  return body_;
}

}
}