#ifndef MATADOR_REQUEST_HPP
#define MATADOR_REQUEST_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
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
#include <unordered_map>

namespace matador {
namespace http {

class response;

class OOS_HTTP_API request
{
public:
  struct version_t {
    int major = 0;
    int minor = 0;
  };

  struct content_t {
    std::size_t length;
    std::string type;
    std::string md5;
    std::string language;
  };

public:
  http::method_t method() const;
  std::string url() const;
  std::string query() const;
  std::string fragment() const;
  version_t version() const;
  std::string host() const;

  const content_t& content() const;

  const t_string_param_map& headers() const;
  const t_string_param_map& path_params() const;
  const t_string_param_map& query_params() const;

  const std::string& body() const;

private:
  friend class request_parser;
  friend class route_endpoint;

private:
  http::method_t method_;
  std::string url_;
  std::string query_;
  std::string fragment_;
  version_t version_;
  std::string host_;

  content_t content_;

  t_string_param_map headers_;
  t_string_param_map path_params_;
  t_string_param_map query_params_;

  std::string body_;
};

}
}

#endif //MATADOR_REQUEST_HPP
