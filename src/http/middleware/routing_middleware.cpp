#include "matador/http/middleware/routing_middleware.hpp"

#include "matador/http/request.hpp"

namespace matador {
namespace http {
namespace middlewares {

response routing_middleware::process(request &req, const middleware::next_func_t &)
{
  auto route = match(req);

  if (!route.has_value()) {
    log_.error("route %s isn't valid", req.url().c_str());
    return response::not_found();
  } else {
    log_.debug("executing route spec: %s (regex: %s)", route.value()->path_spec().c_str(), route.value()->path_regex().c_str());
    return route.value()->execute(req);
  }
}

routing_middleware::routing_middleware(const routing_engine &router)
  : log_(matador::create_logger("RoutingMiddleware"))
  , router_(router)
{}

optional<routing_engine::route_endpoint_ptr> routing_middleware::match(request &req)
{
  auto route = router_.match(req);

  if (router_.valid(route)) {
    return make_optional(*route);
  } else {
    return nullopt;
  }
}
}
}
}