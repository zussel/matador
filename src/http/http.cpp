
#include "matador/http/http.hpp"

namespace matador {
namespace http {

std::unordered_map<http::method_t, std::string> http::method_string_map_({ /* NOLINT */
  { http::method_t::GET, "GET" },
  { http::method_t::POST, "POST" },
  { http::method_t::PUT, "PUT" },
  { http::method_t::DELETE, "DELETE" },
  { http::method_t::OPTIONS, "OPTIONS" },
  { http::method_t::HEAD, "HEAD" },
  { http::method_t::UNKNOWN, "UNKNOWN" }
});

std::unordered_map<http::status_t, std::string> http::status_string_map_({ /* NOLINT */
  { http::status_t::OK, "HTTP/1.1 200 OK\r\n" },
  { http::status_t::CREATED, "HTTP/1.1 201 Created\\r\\n" },
  { http::status_t::ACCEPTED, "HTTP/1.1 202 Accepted\\r\\n" },
  { http::status_t::NO_CONTENT, "HTTP/1.1 204 No Content\\r\\n" },
  { http::status_t::MULTIPLE_CHOICES, "HTTP/1.1 300 Multiple Choices\\r\\n" },
  { http::status_t::MOVED_PERMANENTLY, "HTTP/1.1 301 Moved Permanently\\r\\n" },
  { http::status_t::MOVED_TEMPORARILY, "HTTP/1.1 302 Moved Temporarily\\r\\n" },
  { http::status_t::NOT_MODIFIED, "HTTP/1.1 304 Not Modified\\r\\n" },
  { http::status_t::BAD_REQUEST, "HTTP/1.1 400 Bad Request\\r\\n" },
  { http::status_t::UNAUTHORIZED, "HTTP/1.1 401 Unauthorized\\r\\n" },
  { http::status_t::FORBIDDEN, "HTTP/1.1 403 Forbidden\\r\\n" },
  { http::status_t::NOT_FOUND, "HTTP/1.1 404 Not Found\\r\\n" },
  { http::status_t::INTERNAL_SERVER_ERROR, "HTTP/1.1 500 Internal Server error\\r\\n" },
  { http::status_t::NOT_IMPLEMENTED, "HTTP/1.1 501 Not Implemented\\r\\n" },
  { http::status_t::BAD_GATEWAY, "HTTP/1.1 502 Bad Gateway\\r\\n" },
  { http::status_t::SERVICE_UNAVAILABLE, "HTTP/1.1 503 Service unavailable\\r\\n" }
});

std::string http::to_string(http::method_t method)
{
  return method_string_map_.at(method);
}

std::string http::to_string(http::status_t status)
{
  return status_string_map_.at(status);
}
}
}
