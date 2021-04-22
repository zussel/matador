#include "matador/http/request.hpp"
#include "matador/http/request_header.hpp"
#include "matador/http/mime_types.hpp"

namespace matador {
namespace http {

request::request(http::method_t method, std::string host, std::string url)
  : method_(method)
  , url_(std::move(url))
  , host_(std::move(host))
{
  headers_.insert(std::make_pair(request_header::HOST, host_));
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

bool request::add_header(const std::string &header, const std::string &value)
{
  return headers_.insert(std::make_pair(header, value)).second;
}

bool request::remove_header(const std::string &header)
{
  return headers_.erase(header) > 0;
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

const t_string_param_map &request::form_data() const
{
  return form_data_;
}

const std::string& request::body() const
{
  return body_;
}

void request::body(const std::string &content)
{
  body_ = content;
  content_.type = mime_types::TEXT_PLAIN;
  content_.length = std::to_string(content.length());
}

const char blank[] = { ' ' };
const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };
const char http_version[] = { 'H', 'T', 'T', 'P', '/', '1', '.', '1'};

std::list<matador::buffer_view> request::to_buffers() const
{
  std::list<matador::buffer_view> buffers;
  buffers.push_back(http::to_buffer(method_));
  buffers.emplace_back(matador::buffer_view(blank, 1));
  buffers.emplace_back(url_);
  buffers.emplace_back(matador::buffer_view(blank, 1));
  buffers.emplace_back(matador::buffer_view(http_version, 8));
  buffers.emplace_back(matador::buffer_view(crlf, 2));

  for(const auto &p : headers_) {
    buffers.emplace_back(p.first);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(p.second);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
  }

  if (!body_.empty()) {
    buffers.emplace_back(request_header::CONTENT_LENGTH);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(content_.length);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
    buffers.emplace_back(request_header::CONTENT_TYPE);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(content_.type);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
  }

  buffers.emplace_back(matador::buffer_view(crlf, 2));

  if (!body_.empty()) {
    buffers.emplace_back(body_);
  }

  return buffers;
}

}
}