
#include "matador/http/http.hpp"

namespace matador {
namespace http {

std::unordered_map<http::method_t, std::string, detail::enum_class_hash> http::method_string_map_{{ /* NOLINT */
  { http::method_t::GET, "GET" },
  { http::method_t::POST, "POST" },
  { http::method_t::PUT, "PUT" },
  { http::method_t::DEL, "DELETE" },
  { http::method_t::OPTIONS, "OPTIONS" },
  { http::method_t::HEAD, "HEAD" },
  { http::method_t::UNKNOWN, "UNKNOWN" }
}};

std::unordered_map<std::string, http::method_t> http::string_method_map_{{ /* NOLINT */
  { "GET", http::method_t::GET },
  { "POST", http::method_t::POST, },
  { "PUT", http::method_t::PUT, },
  { "DELETE", http::method_t::DEL, },
  { "OPTIONS", http::method_t::OPTIONS, },
  { "HEAD", http::method_t::HEAD, },
  { "UNKNOWN", http::method_t::UNKNOWN, }
}};

std::unordered_map<http::status_t, std::string, detail::enum_class_hash> http::request_status_string_map_{{ /* NOLINT */
  { http::status_t::OK, "HTTP/1.1 200 OK\r\n" },
  { http::status_t::CREATED, "HTTP/1.1 201 Created\r\n" },
  { http::status_t::ACCEPTED, "HTTP/1.1 202 Accepted\r\n" },
  { http::status_t::NO_CONTENT, "HTTP/1.1 204 No Content\r\n" },
  { http::status_t::MULTIPLE_CHOICES, "HTTP/1.1 300 Multiple Choices\r\n" },
  { http::status_t::MOVED_PERMANENTLY, "HTTP/1.1 301 Moved Permanently\r\n" },
  { http::status_t::MOVED_TEMPORARILY, "HTTP/1.1 302 Moved Temporarily\r\n" },
  { http::status_t::NOT_MODIFIED, "HTTP/1.1 304 Not Modified\r\n" },
  { http::status_t::BAD_REQUEST, "HTTP/1.1 400 Bad Request\r\n" },
  { http::status_t::UNAUTHORIZED, "HTTP/1.1 401 Unauthorized\r\n" },
  { http::status_t::FORBIDDEN, "HTTP/1.1 403 Forbidden\r\n" },
  { http::status_t::NOT_FOUND, "HTTP/1.1 404 Not Found\r\n" },
  { http::status_t::INTERNAL_SERVER_ERROR, "HTTP/1.1 500 Internal Server error\r\n" },
  { http::status_t::NOT_IMPLEMENTED, "HTTP/1.1 501 Not Implemented\r\n" },
  { http::status_t::BAD_GATEWAY, "HTTP/1.1 502 Bad Gateway\r\n" },
  { http::status_t::SERVICE_UNAVAILABLE, "HTTP/1.1 503 Service unavailable\r\n" }
}};

std::unordered_map<http::status_t, std::string, detail::enum_class_hash> http::status_string_map_{{ /* NOLINT */
  { http::status_t::OK, "OK" },
  { http::status_t::CREATED, "Created" },
  { http::status_t::ACCEPTED, "Accepted" },
  { http::status_t::NO_CONTENT, "No Content" },
  { http::status_t::MULTIPLE_CHOICES, "Multiple Choices" },
  { http::status_t::MOVED_PERMANENTLY, "Moved Permanently" },
  { http::status_t::MOVED_TEMPORARILY, "Moved Temporarily" },
  { http::status_t::NOT_MODIFIED, "Not Modified" },
  { http::status_t::BAD_REQUEST, "Bad Request" },
  { http::status_t::UNAUTHORIZED, "Unauthorized" },
  { http::status_t::FORBIDDEN, "Forbidden" },
  { http::status_t::NOT_FOUND, "Not Found" },
  { http::status_t::INTERNAL_SERVER_ERROR, "Internal Server error" },
  { http::status_t::NOT_IMPLEMENTED, "Not Implemented" },
  { http::status_t::BAD_GATEWAY, "Bad Gateway" },
  { http::status_t::SERVICE_UNAVAILABLE, "Service unavailable" }
}};

std::unordered_map<std::string, http::status_t> http::string_status_map_{{ /* NOLINT */
  { "200", http::OK },
  { "201", http::CREATED },
  { "202", http::ACCEPTED },
  { "204", http::NO_CONTENT },
  { "300", http::MULTIPLE_CHOICES },
  { "301", http::MOVED_PERMANENTLY },
  { "302", http::MOVED_TEMPORARILY },
  { "304", http::NOT_MODIFIED },
  { "400", http::BAD_REQUEST },
  { "401", http::UNAUTHORIZED },
  { "403", http::FORBIDDEN },
  { "404", http::NOT_FOUND },
  { "500", http::INTERNAL_SERVER_ERROR },
  { "501", http::NOT_IMPLEMENTED },
  { "502", http::BAD_GATEWAY },
  { "503", http::SERVICE_UNAVAILABLE }
}};

std::string http::to_string(http::method_t method)
{
  return method_string_map_.at(method);
}

matador::buffer_view http::to_buffer(http::method_t method)
{
  return matador::buffer_view(method_string_map_.at(method));
}

http::method_t http::to_method(const std::string &str)
{
  auto it = string_method_map_.find(str);
  if (it == string_method_map_.end()) {
    return UNKNOWN;
  }
  return it->second;
}

std::string http::to_request_string(http::status_t status)
{
  return request_status_string_map_.at(status);
}

std::string http::to_string(http::status_t status)
{
  return status_string_map_.at(status);
}

matador::buffer_view http::to_buffer(http::status_t status)
{
  return matador::buffer_view(request_status_string_map_.at(status));
}

http::status_t http::to_status(const std::string &str)
{
  return string_status_map_.at(str);
}

}
}
