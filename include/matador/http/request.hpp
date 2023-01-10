#ifndef MATADOR_REQUEST_HPP
#define MATADOR_REQUEST_HPP

#include "matador/http/export.hpp"

#include "matador/http/http.hpp"

#include <string>
#include <list>
#include <unordered_map>

namespace matador {
namespace http {

class response;

/**
 * The request class bundles all information
 * of a HTTP request to send.
 */
class OOS_HTTP_API request
{
public:
  /**
   * Default constructor
   */
  request() = default;

  /**
   * Initializes a request with the given
   * HTTP method, host and port.
   *
   * @param method HTTP method
   * @param host Host of the request
   * @param url Port of the request
   */
  request(http::method_t method, std::string host, std::string url);

  /// @cond MATADOR_DEV
  request(const request &) = default;
  request& operator=(const request &) = default;
  request(request &&) = default;
  request& operator=(request &&) = default;
  ~request() = default;
  /// @endcond

  /**
   * Returns the HTTP method of the request
   *
   * @return The HTTP method
   */
  http::method_t method() const;

  /**
   * Returns the URL of the request.
   *
   * @return The URL of the request.
   */
  std::string url() const;

  /**
   * Returns the fragment of the request.
   * This is the anchor part in the URL
   * i.e. http://localhost/page#fragment
   *
   * @return The fragment of the request
   */
  std::string fragment() const;

  /**
   * Returns the requests HTTP version.
   *
   * @return The used HTTP version
   */
  http::version version() const;

  /**
   * Returns the host of the request.
   *
   * @return The host of the request
   */
  std::string host() const;

  /**
   * Adds a HTTP header to the request consisting of the
   * header key and the header value.
   *
   * @param header The header key name
   * @param value The value of the key
   * @return True if the header was successfully added.
   */
  bool add_header(const std::string &header, const std::string &value);

  /**
   * Removes a header identified by the given
   * header key from the request.
   *
   * @param header Key of the header to remove
   * @return True if the header was successfully removed
   */
  bool remove_header(const std::string &header);

  /**
   * returns the content information of the request
   * consisting of
   * - length
   * - type
   * - md5
   * - language
   *
   * @return The requests content information
   */
  const http::content& content() const;

  /**
   * Return the requests HTTP header.
   *
   * @return The HTTP header
   */
  const t_string_param_map& headers() const;

  /**
   * Return the requets path param map.
   * If the url has captures for path params
   * they are store in the path param map
   * and can be obtained by their name.
   *
   * @return The path parameter map
   */
  const t_string_param_map& path_params() const;

  /**
   * Return the requests HTTP query params as
   * a key value map.
   *
   * @return The query parameter map
   */
  const t_string_param_map& query_params() const;

  /**
   * Return the form data map of the request.
   *
   * @return The form data map
   */
  const t_string_param_map& form_data() const;

  /**
   * Returns the raw body as string.
   *
   * @return The body of the request
   */
  const std::string& body() const;

  /**
   * Sets the body of the request.
   *
   * @param content The body content to set
   */
  void body(const std::string &content);

  /**
   * Creates a list of view buffer containing
   * the request ready to be send.
   *
   * @return List of buffer views containing the request ready to send
   */
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
