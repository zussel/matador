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

std::map<log_level, std::string> log_domain::level_strings = {
  { log_level::LVL_DEBUG, "DEBUG" },
  { log_level::LVL_INFO, "INFO" },
  { log_level::LVL_WARN, "WARN" },
  { log_level::LVL_ERROR, "ERROR" },
  { log_level::LVL_TRACE, "TRACE" }
};

log_domain::log_domain(std::string name)
  : name_(std::move(name))
{}

//log_domain::log_domain(std::string name, sink_ptr sink)
//  : name_(std::move(name))
//{
//  sinks.push_back(std::move(sink));
//}

std::string log_domain::name() const
{
  return name_;
}

void log_domain::add_sink(sink_ptr sink)
{
  sinks.push_back(std::move(sink));
}

void log_domain::log(log_level lvl, const std::string &source, const char *message)
{
  char timestamp_buffer [80];

  const char *timestamp = details::gettimestamp(timestamp_buffer, 80);

  char buffer[1024];

#ifdef _MSC_VER
  int ret = sprintf_s(buffer, 1024, "%s [%-7s][%s]: %s\n", timestamp, level_strings[lvl].c_str(), source.c_str(), message);
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
