
#include "matador/http/http.hpp"

namespace matador {
namespace http {

const char* to_string(http::method_t method)
{
  switch (method) {
    case http::GET:
      return "GET";
    case http::POST:
      return "POST";
    case http::PUT:
      return "PUT";
    case http::DELETE:
      return "DELETE";
    case http::OPTIONS:
      return "OPTIONS";
    case http::HEAD:
      return "HEAD";
    case http::UNKNOWN:
    default:
      return "UNKNOWN";
  }
}

}
}
