#ifndef MATADOR_ROUTING_MIDDLEWARE_HPP
#define MATADOR_ROUTING_MIDDLEWARE_HPP

#include "matador/logger/log_manager.hpp"

#include "matador/utils/optional.hpp"

#include "matador/http/middleware.hpp"
#include "matador/http/routing_engine.hpp"

namespace matador {
namespace http {
namespace middlewares {

class routing_middleware : public middleware
{
public:
  explicit routing_middleware(const routing_engine &router);

  response process(request &req, const next_func_t &next) override;

private:
  optional<routing_engine::route_endpoint_ptr> match(request &req);

private:
  matador::logger log_;

  const matador::http::routing_engine &router_;
};

}
}
}
#endif //MATADOR_ROUTING_MIDDLEWARE_HPP
