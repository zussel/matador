#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

namespace matador {

std::map<logger::level, std::string> logger::level_strings = {
{ LVL_DEBUG, "DEBUG" },
{ LVL_INFO, "INFO" },
{ LVL_WARN, "WARN" },
{ LVL_ERROR, "ERROR" },
{ LVL_TRACE, "TRACE" }
};

logger::logger(const char *source, log_manager &manager)
: source_(source), manager_(manager)
{}

const char *logger::gettimestamp(char *buffer, size_t size) const
{
  struct timeval tp{};
  matador::gettimeofday(&tp);
  matador::strftime(buffer, size, TIMESTAMP_FORMAT, &tp);
  return buffer;
}

void logger::log(const char *timestamp, const char *level, const char *message) {
  char buffer[1024];

  int ret = sprintf(buffer, "%s $$ %-7s $$ %s $$ %s\n", timestamp, level, source_, message);

  manager_.log(buffer, ret);
}

}