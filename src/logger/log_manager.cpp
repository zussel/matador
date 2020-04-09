#include "matador/logger/log_manager.hpp"

namespace matador {

logger log_manager::create_logger(const char *source)
{
  return logger(source, *this);
}

void log_manager::add(std::unique_ptr<log_sink> sink)
{
  sinks.push_back(std::move(sink));
}

void log_manager::log(const char *msg, size_t size)
{
  for (auto &sink : sinks) {
    sink->write(msg, size);
  }
}

}
