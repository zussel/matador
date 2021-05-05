#ifndef MATADOR_MIDDLEWARE_HPP
#define MATADOR_MIDDLEWARE_HPP

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

#include "matador/http/response.hpp"

#include <functional>
#include <memory>

namespace matador {
namespace http {

class request;

class OOS_HTTP_API middleware
{
public:
  using next_func_t = std::function<matador::http::response()>;
  virtual ~middleware() = default;

  virtual matador::http::response process(matador::http::request &req, const next_func_t &next) = 0;
};

using middleware_ptr = std::shared_ptr<middleware>;

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

}
}
#endif //MATADOR_MIDDLEWARE_HPP
