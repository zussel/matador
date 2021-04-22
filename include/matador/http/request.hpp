#ifndef MATADOR_REQUEST_HPP
#define MATADOR_REQUEST_HPP

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

#include <string>
#include <list>
#include <unordered_map>

namespace matador {
namespace http {

class response;

class OOS_HTTP_API request
{
public:
  request() = default;
  request(http::method_t method, std::string host, std::string url);
  request(const request &) = default;
  request& operator=(const request &) = default;
  request(request &&) = default;
  request& operator=(request &&) = default;
  ~request() = default;

  http::method_t method() const;
  std::string url() const;
  std::string query() const;
  std::string fragment() const;
  http::version version() const;
  std::string host() const;

  bool add_header(const std::string &header, const std::string &value);
  bool remove_header(const std::string &header);

  const http::content& content() const;

  const t_string_param_map& headers() const;
  const t_string_param_map& path_params() const;
  const t_string_param_map& query_params() const;
  const t_string_param_map& form_data() const;

  const std::string& body() const;
  void body(const std::string &content);

  std::list<matador::buffer_view> to_buffers() const;


private:
  friend class request_parser;
  friend class route_endpoint;

private:
  http::method_t method_ = http::UNKNOWN;
  std::string url_;
  std::string query_;
  std::string fragment_;
  http::version version_;
  std::string host_;

  http::content content_;

  t_string_param_map headers_;
  t_string_param_map path_params_;
  t_string_param_map query_params_;
  t_string_param_map form_data_;

  std::string body_;
};

}
}

#endif //MATADOR_REQUEST_HPP
