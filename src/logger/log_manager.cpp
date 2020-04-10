#include "matador/logger/log_manager.hpp"

namespace matador {

logger log_manager::create_logger(std::string source)
{
  return logger(std::move(source), default_log_domain_);
}

logger log_manager::create_logger(std::string source, const std::string &domain_name)
{
  return logger(std::move(source), default_log_domain_);
}

void log_manager::add_sink(sink_ptr sink)
{
  default_log_domain_->add_sink(std::move(sink));
}
}
