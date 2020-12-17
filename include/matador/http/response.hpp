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

namespace matador {
namespace http {

class OOS_HTTP_API response
{
  typedef std::unordered_map<std::string, std::string> t_string_string_map;

  struct version_t {
    int major = 0;
    int minor = 0;
  };

  struct content_t {
    std::size_t length = 0;
    std::string type;
  };

public:

  template < class T >
  static response json(const T &obj);

  template < class T >
  static response json(http::status_t status, const T &obj);

  static response no_content();
  static response not_found();
  static response bad_request();

  template < class T >
  static response not_found(const T &obj);

  std::string to_string() const;

private:
  static response create(http::status_t status);

public:
  version_t version;
  content_t content_type;

  http::status_t status = http::OK;

  t_string_string_map headers;

  std::string body;
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

  resp.body = js.to_json(obj);
  resp.content_type.type = mime_types::APPLICATION_JSON;

  resp.content_type.length = resp.body.size();

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
