#ifndef MATADOR_RESPONSE_HPP
#define MATADOR_RESPONSE_HPP

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

#include <unordered_map>
#include <string>

#include "matador/http/http.hpp"
#include "matador/http/mime_types.hpp"
#include "matador/http/response_header.hpp"

#include "matador/utils/json_mapper.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/file.hpp"
#include "matador/utils/buffer_view.hpp"

namespace matador {

class json;

namespace http {

/**
 * The response class bundles all information of the
 * result of an HTTP request.
 */
class OOS_HTTP_API response
{
public:
  /**
   * Default constructor
   */
  response() = default;

  /// @cond MATADOR_DEV
  response(const response &) = default;
  response& operator=(const response &) = default;
  response(response &&) = default;
  response& operator=(response &&) = default;
  ~response() = default;
  /// @endcond

  /**
   * Returns the HTTP status of the reponse
   *
   * @return The status of the response
   */
  http::status_t status() const;

  /**
   * Returns the HTTP version of the response
   *
   * @return The HTTP version of the response
   */
  http::version version() const;

  /**
   * returns the content information of the response
   * consisting of
   * - length
   * - type
   * - md5
   * - language
   *
   * @return The response content information
   */
  const http::content& content() const;

  /**
   * Return the response HTTP header.
   *
   * @return The HTTP header
   */
  const t_string_param_map& headers() const;

  /**
   * Returns the raw body as string.
   *
   * @return The body of the response
   */
  const std::string& body() const;

  /**
   * Creates a OK response with the given object
   * converted to a json string as body
   *
   * @tparam T Type of the object
   * @param obj Object to be converted as json body
   * @return The created OK response
   */
  template < class T >
  static response json(const T &obj);

  /**
   * Creates a response with the given status and
   * object converted to a json string as body
   *
   * @tparam T Type of the object
   * @param status Status of the response to create
   * @param obj Object to be converted as json body
   * @return The created response
   */
  template < class T >
  static response json(http::status_t status, const T &obj);

  static response ok(const matador::json &body);
  static response ok(const std::string &body, mime_types::types type);
  static response no_content();
  static response not_found();
  static response bad_request();
  static response redirect(const std::string &location);

  template < class T >
  static response not_found(const T &obj);

  static response from_file(const std::string &file_path);

  /**
   * Returns the response as a string
   *
   * @return Response as string
   */
  std::string to_string() const;

  /**
   * Creates a list of view buffer containing
   * the response ready to be send.
   *
   * @return List of buffer views containing the response ready to send
   */
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

  json_mapper mapper;

  resp.body_ = mapper.to_string(obj, json_format::compact);
  resp.content_.type = mime_types::APPLICATION_JSON;

  resp.content_.length = std::to_string(resp.body_.size());

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
