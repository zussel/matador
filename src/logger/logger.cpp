#include "matador/logger/logger.hpp"

namespace matador {

logger::logger(std::string source, std::shared_ptr<log_domain> log_domain)
  : source_(std::move(source))
  , logger_domain_(std::move(log_domain))
{}

logger::logger(logger&& l) noexcept
: source_(std::move(l.source_))
, logger_domain_(std::move(l.logger_domain_))
{}

logger& logger::operator=(logger&& l) noexcept {
  source_ = std::move(l.source_);
  logger_domain_ = std::move(l.logger_domain_);
  return *this;
}

const std::string& logger::source() const
{
  return source_;
}

std::string logger::domain() const
{
  return logger_domain_->name();
}

void logger::log(log_level lvl, const char *what)
{
  logger_domain_->log(lvl, source_, what);
}
}