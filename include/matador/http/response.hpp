#ifndef MATADOR_RESPONSE_HPP
#define MATADOR_RESPONSE_HPP

#include "matador/http/export.hpp"

#include <unordered_map>
#include <string>

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/json_object_mapper.hpp"

#include "matador/http/http.hpp"
#include "matador/http/mime_types.hpp"
#include "matador/http/response_header.hpp"

#include "matador/json/json_mapper.hpp"
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
   * Creates an OK response with the given object
   * converted to a json string as body
   *
   * @tparam T Type of the object
   * @param obj Object to be converted as json body
   * @return The created OK response
   */
  template < class T >
  static response ok(const T &obj);

  /**
   * Creates an OK response with the give object_ptr
   * converted into a json string as body
   *
   * @tparam T Type of the object
   * @tparam OPT Type of the object_pointer
   * @param obj Object to convert
   * @return The created OK response
   */
  template < class T, object_holder_type OPT >
  static response ok(const object_pointer<T, OPT> &obj);

  /**
   * Creates an OK response with the give object_view
   * of objects of type T converted into a json
   * string as body
   *
   * @tparam T Type of the object in the view
   * @param view The object view to convert
   * @return The created OK response
   */
  template<class T>
  static response ok(const object_view<T> &view);

  /**
   * Creates an OK response of the given json
   * object as body.
   *
   * @param body The body data as json object
   * @return The created OK response
   */
  static response ok(const matador::json &body);

  /**
   * Creates an OK response with the given body
   * string and the given media type
   *
   * @param body Body as string
   * @param type Media type of body
   * @return The created OK response
   */
  static response ok(const std::string &body, mime_types::types type);

  /**
   * Creates a NO_CONTENT response
   *
   * @return The created NO_CONTENT response
   */
  static response no_content();

  /**
   * Creates a NOT_FOUND response
   *
   * @return The created NOT_FOUND response
   */
  static response not_found();

  /**
   * Creates a NOT_FOUND response with the
   * given object converted to string as
   * body
   *
   * @tparam T Type of the object
   * @param obj Object to be converted
   * @return The created NOT_FOUND response
   */
  template < class T >
  static response not_found(const T &obj);

  /**
   * Creates a BAD_REQUEST response
   *
   * @return The created BAD_REQUEST response
   */
  static response bad_request();

  /**
   * Creates a REDIRECT response to the given
   * location.
   *
   * @param location Location to redirect to
   * @return The created REDIRECT response
   */
  static response redirect(const std::string &location);

  /**
   * Creates an OK response from a file
   * at the given path. The media type is
   * determined by the file extension. If unknown
   * media type is PLAN_TEXT.
   *
   * @param file_path Path of the file
   * @return The created OK response
   */
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
  static response create(http::status_t status, const std::string &body, mime_types::types type);

private:
  friend class response_parser;

  http::version version_;
  http::content content_;

  http::status_t status_ = http::OK;

  t_string_param_map headers_;

  std::string body_;
};

template<class T>
response response::ok(const T &obj)
{
  json_mapper mapper;

  return create(http::OK, mapper.to_string(obj, json_format::compact), mime_types::TYPE_APPLICATION_JSON);
}

template<class T, object_holder_type OPT>
response response::ok(const object_pointer <T, OPT> &obj)
{
  json_object_mapper mapper;

  return create(http::OK, mapper.to_string(obj, json_format::compact), mime_types::TYPE_APPLICATION_JSON);
}

template<class T>
response response::ok(const object_view<T> &view)
{
  json_object_mapper mapper;

  return create(http::OK, mapper.to_string(view, json_format::compact), mime_types::TYPE_APPLICATION_JSON);
}

template<class T>
response response::not_found(const T &obj)
{
  json_mapper mapper;

  return create(http::NOT_FOUND, mapper.to_string(obj, json_format::compact), mime_types::APPLICATION_JSON);
}

}
}

#endif //MATADOR_RESPONSE_HPP
