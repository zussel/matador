#include <matador/utils/time.hpp>
#include "matador/logger/logger_domain.hpp"

namespace matador {

namespace details {

const char *gettimestamp(char *buffer, size_t size)
{
  struct timeval tp{};
  matador::gettimeofday(&tp);
  matador::strftime(buffer, size, logger_domain::TIMESTAMP_FORMAT, &tp);
  return buffer;
}

}

std::map<log_level, std::string> logger_domain::level_strings = {
  { log_level::LVL_DEBUG, "DEBUG" },
  { log_level::LVL_INFO, "INFO" },
  { log_level::LVL_WARN, "WARN" },
  { log_level::LVL_ERROR, "ERROR" },
  { log_level::LVL_TRACE, "TRACE" }
};


logger_domain::logger_domain(std::string name, sink_ptr sink)
  : name_(std::move(name))
{
  sinks.push_back(std::move(sink));
}

std::string logger_domain::name() const
{
  return name_;
}

void logger_domain::add_sink(sink_ptr sink)
{
  sinks.push_back(std::move(sink));
}

void logger_domain::log(log_level lvl, const std::string &source, const char *message)
{
  char timestamp_buffer [80];

  const char *timestamp = details::gettimestamp(timestamp_buffer, 80);

  char buffer[1024];

  int ret = sprintf(buffer, "%s $$ %-7s $$ %s $$ %s\n", timestamp, level_strings[lvl].c_str(), source.c_str(), message);

  for (auto &sink : sinks) {
    sink->write(buffer, ret);
  }
}

}
