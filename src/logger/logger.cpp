#include "matador/logger/logger.hpp"

namespace matador {

logger::logger(std::string source, std::shared_ptr<logger_domain> log_domain)
  : source_(std::move(source))
  , logger_domain_(std::move(log_domain))
{}

}