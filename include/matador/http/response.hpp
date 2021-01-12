#ifndef MATADOR_RESPONSE_HPP
#define MATADOR_RESPONSE_HPP

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

#include <unordered_map>
#include <string>

#include "matador/http/http.hpp"
#include "matador/http/mime_types.hpp"
#include "matador/http/response_header.hpp"

#include "matador/utils/json_serializer.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/file.hpp"
#include "matador/utils/buffer_view.hpp"

namespace matador {
namespace http {

class OOS_HTTP_API response
{
public:
  response() = default;
  response(const response &) = default;
  response& operator=(const response &) = default;
  response(response &&) = default;
  response& operator=(response &&) = default;
  ~response() = default;

  http::status_t status() const;
  http::version version() const;
  const http::content& content() const;
  const t_string_param_map& headers() const;
  const std::string& body() const;

  template < class T >
  static response json(const T &obj);

  template < class T >
  static response json(http::status_t status, const T &obj);

  static response no_content();
  static response not_found();
  static response bad_request();

  template < class T >
  static response not_found(const T &obj);

  static response from_file(const std::string &file_path);

  std::string to_string() const;

  std::list<matador::buffer_view> to_buffers() const;

private:
  static response create(http::status_t status);

private:
  friend class response_parser;

  http::version version_;
  http::content content_;

  http::status_t status_ = http::OK;

  t_string_param_map headers_;

  std::string body_;
};

template<class T>
response response::json(const T &obj)
{
  return json(http::OK, obj);
}

template<class T>
response response::json(http::status_t status, const T &obj)
{
  response resp = create(status);

  json_serializer js(json_format::compact);

  resp.body_ = js.to_json(obj);
  resp.content_.type = mime_types::APPLICATION_JSON;

  resp.content_.length = resp.body_.size();

  return resp;
}

template<class T>
response response::not_found(const T &obj)
{
  return json(http::NOT_FOUND, obj);
}

}
}

#endif //MATADOR_RESPONSE_HPP
