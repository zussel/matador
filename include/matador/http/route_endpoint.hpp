#ifndef MATADOR_ROUTE_ENDPOINT_HPP
#define MATADOR_ROUTE_ENDPOINT_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include "matador/http/http.hpp"

#include <functional>
#include <regex>

namespace matador {
namespace http {

class request;
class response;

typedef std::function<response(const request&)> t_request_handler;
typedef std::unordered_map<size_t, std::string> t_size_string_map;

/*
 * path
 * path_regex
 * method
 */
class OOS_HTTP_API route_endpoint
{
public:
  route_endpoint(std::string path_spec, const std::string& path_regex, http::method_t method, t_request_handler request_handler, t_size_string_map param_index_map);

  bool match(request &req);

  http::method_t method() const;
  const std::string& path_spec() const;
  const std::string& path_regex() const;
  const t_size_string_map& param_map() const;

  response execute(const request &req);

private:
  std::string path_spec_;
  std::regex path_regex_;
  std::string path_regex_str_;
  http::method_t method_;
  t_request_handler request_handler_;

  t_size_string_map param_index_map_;
};

}
}
#endif //MATADOR_ROUTE_ENDPOINT_HPP
