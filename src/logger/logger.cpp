#include "matador/logger/logger.hpp"

namespace matador {

logger::logger(std::string source, std::shared_ptr<log_domain> log_domain)
  : source_(std::move(source))
  , logger_domain_(std::move(log_domain))
{}

std::string logger::source() const
{
  return source_;
}

std::string logger::domain() const
{
  return logger_domain_->name();
}

}