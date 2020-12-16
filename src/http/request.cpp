#include "matador/http/request.hpp"

namespace matador {
namespace http {

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

request::version_t request::version() const
{
  return version_;
}

std::string request::host() const
{
  return host_;
}

const request::content_t& request::content() const
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