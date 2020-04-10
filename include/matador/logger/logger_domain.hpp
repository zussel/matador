#ifndef MATADOR_LOGGER_DOMAIN_HPP
#define MATADOR_LOGGER_DOMAIN_HPP

#include "matador/logger/log_sink.hpp"
#include "matador/logger/log_level.hpp"

#include <string>
#include <list>
#include <map>

namespace matador {

class logger_domain
{
public:
  static constexpr const char *TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S.%f";

  logger_domain() = default;
  logger_domain(std::string name, sink_ptr sink);

  std::string name() const;

  void add_sink(sink_ptr sink);

  void log(log_level lvl, const std::string &source, const char *message);
private:

  static std::map<log_level, std::string> level_strings;

  std::string name_;
  std::list<sink_ptr> sinks;
};

}
#endif //MATADOR_LOGGER_DOMAIN_HPP
