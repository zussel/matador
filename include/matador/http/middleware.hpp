#ifndef MATADOR_MIDDLEWARE_HPP
#define MATADOR_MIDDLEWARE_HPP

#include "matador/http/response.hpp"

#include <functional>
#include <memory>

namespace matador {
namespace http {

class request;

class middleware
{
public:
  using next_func_t = std::function<matador::http::response()>;
  virtual ~middleware() = default;

  virtual matador::http::response process(matador::http::request &req, const next_func_t &next) = 0;
};

using middleware_ptr = std::shared_ptr<middleware>;

class middleware_processor
{
public:
  explicit middleware_processor(middleware_ptr current, std::shared_ptr<middleware_processor> next);

  matador::http::response process(matador::http::request &req);

private:
  middleware_ptr current_;
  std::shared_ptr<middleware_processor> next_;
};

class sentinel_middleware : public middleware
{
public:
  matador::http::response process(request &, const next_func_t &) override;
};

class middleware_pipeline
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
