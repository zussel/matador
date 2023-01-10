#ifndef MATADOR_MIDDLEWARE_HPP
#define MATADOR_MIDDLEWARE_HPP

#include "matador/http/export.hpp"

#include "matador/http/response.hpp"

#include <functional>
#include <memory>

namespace matador {
namespace http {

class request;

/**
 * The middleware class is used when processing
 * a HTTP request and works like onion layers
 * around a request.
 *
 * The interface consists only of one method process.
 * Taking the incoming request and the next middleware
 * callback object and returning the resulting
 * response object.
 *
 * With these incoming parameters the implementation
 * has the ability to process the request before the next
 * middleware has processed it and process the request
 * after the next middleware has processed the request.
 *
 * The same applies for the response object.
 */
class OOS_HTTP_API middleware
{
public:
  /**
   * Shortcut to the next middleware callback
   */
  using next_func_t = std::function<matador::http::response()>;

  /**
   * Destructor
   */
  virtual ~middleware() = default;

  /**
   * This method must implement the processing of the
   * incoming request, call the next middleware processing
   * and return a response object.
   *
   * @param req Incoming request to process
   * @param next Callback to the succeeding middleware
   * @return The response
   */
  virtual matador::http::response process(matador::http::request &req, const next_func_t &next) = 0;
};

using middleware_ptr = std::shared_ptr<middleware>;

/// @cond MATADOR_DEV

class OOS_HTTP_API middleware_processor
{
public:
  explicit middleware_processor(middleware_ptr current, std::shared_ptr<middleware_processor> next);

  matador::http::response process(matador::http::request &req);

private:
  middleware_ptr current_;
  std::shared_ptr<middleware_processor> next_;
};

class OOS_HTTP_API sentinel_middleware : public middleware
{
public:
  matador::http::response process(request &, const next_func_t &) override;
};

class OOS_HTTP_API middleware_pipeline
{
private:
  using middleware_processor_ptr = std::shared_ptr<middleware_processor>;

public:
  middleware_pipeline();

  void add(const middleware_ptr &mware);

  matador::http::response process(matador::http::request &req);

private:
  middleware_processor_ptr processor_;
};

/// @endcond

}
}
#endif //MATADOR_MIDDLEWARE_HPP
