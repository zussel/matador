#include "matador/http/middleware.hpp"

namespace matador {
namespace http {

middleware_processor::middleware_processor(middleware_ptr current, std::shared_ptr<middleware_processor> next)
  : current_(std::move(current))
  , next_(std::move(next))
{}

matador::http::response middleware_processor::process(request &req)
{
  return current_->process(req, [&req, this]() {
    if (next_) {
      return next_->process(req);
    } else {
      return matador::http::response::not_found();
    }
  });
}

middleware_pipeline::middleware_pipeline()
  : processor_(std::make_shared<middleware_processor>(std::make_shared<sentinel_middleware>(), middleware_processor_ptr()))
{}

void middleware_pipeline::add(const middleware_ptr &mware)
{
  processor_ = std::make_shared<middleware_processor>(mware, processor_);
}

matador::http::response middleware_pipeline::process(request &req)
{
  return processor_->process(req);
}

matador::http::response sentinel_middleware::process(request &, const middleware::next_func_t &)
{
  return matador::http::response::bad_request();
}

}
}
