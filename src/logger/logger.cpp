#include "matador/logger/logger.hpp"

namespace matador {

logger::logger(std::string source, std::shared_ptr<log_domain> log_domain)
  : source_(std::move(source))
  , logger_domain_(std::move(log_domain))
{}

logger::logger(const logger& l)
: source_(l.source_)
, logger_domain_(l.logger_domain_)
{}

logger& logger::operator=(const logger& l)
{
  source_ = l.source_;
  std::lock_guard<std::mutex> lock(l.mutex_);
  logger_domain_ = l.logger_domain_;
  return *this;
}

std::string logger::source() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return source_;
}

std::string logger::domain() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return logger_domain_->name();
}

void logger::log(log_level lvl, const char *what)
{
  std::lock_guard<std::mutex> lock(mutex_);
  logger_domain_->log(lvl, source_, what);
}
}