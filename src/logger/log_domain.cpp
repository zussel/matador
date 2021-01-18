#include "matador/logger/log_domain.hpp"

#include "matador/utils/time.hpp"

namespace matador {

namespace details {

const char *gettimestamp(char *buffer, size_t size)
{
  struct timeval tp{};
  matador::gettimeofday(&tp);
  matador::strftime(buffer, size, log_domain::TIMESTAMP_FORMAT, &tp);
  return buffer;
}

}

std::map<log_level, std::string> log_domain::level_strings = { /* NOLINT */
  { log_level::LVL_DEBUG, "DEBUG" },
  { log_level::LVL_INFO, "INFO" },
  { log_level::LVL_WARN, "WARN" },
  { log_level::LVL_ERROR, "ERROR" },
  { log_level::LVL_TRACE, "TRACE" }
};

log_domain::log_domain(std::string name, log_level_range log_range)
  : name_(std::move(name))
  , log_level_range_(log_range)
{}

std::string log_domain::name() const
{
  return name_;
}

void log_domain::max_log_level(log_level max_level)
{
  log_level_range_.max_level = max_level;
}

log_level log_domain::max_log_level() const
{
  return log_level_range_.max_level;
}

void log_domain::min_log_level(log_level min_level)
{
  log_level_range_.min_level = min_level;
}

log_level log_domain::min_log_level() const
{
  return log_level_range_.min_level;
}

void log_domain::add_sink(sink_ptr sink)
{
  sinks.push_back(std::move(sink));
}

void log_domain::log(log_level lvl, const std::string &source, const char *message)
{
  if (lvl < log_level_range_.max_level || lvl > log_level_range_.min_level) {
    return;
  }

  char timestamp_buffer [80];

  const char *timestamp = details::gettimestamp(timestamp_buffer, 80);

  char buffer[1024];

#ifdef _MSC_VER
  int ret = sprintf_s(buffer, 1024, "%s [%-7s] [%s]: %s\n", timestamp, level_strings[lvl].c_str(), source.c_str(), message);
#else
  int ret = sprintf(buffer, "%s [%-7s] [%s]: %s\n", timestamp, level_strings[lvl].c_str(), source.c_str(), message);
#endif

  for (auto &sink : sinks) {
    sink->write(buffer, ret);
  }
}

void log_domain::clear()
{
  sinks.clear();
}

}
