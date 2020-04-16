#ifndef MATADOR_LOG_DOMAIN_HPP
#define MATADOR_LOG_DOMAIN_HPP

#ifdef _MSC_VER
#ifdef matador_logger_EXPORTS
#define OOS_LOGGER_API __declspec(dllexport)
#define EXPIMP_LOGGER_TEMPLATE
#else
#define OOS_LOGGER_API __declspec(dllimport)
#define EXPIMP_LOGGER_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_LOGGER_API
#endif

#include "matador/logger/log_sink.hpp"
#include "matador/logger/log_level.hpp"

#include <string>
#include <list>
#include <map>

namespace matador {

class OOS_LOGGER_API log_domain
{
public:
  static constexpr const char *TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S.%f";

  explicit log_domain(std::string name);
  log_domain(std::string name, sink_ptr sink);

  std::string name() const;

  void add_sink(sink_ptr sink);

  void log(log_level lvl, const std::string &source, const char *message);

  void clear();
private:

  static std::map<log_level, std::string> level_strings;

  std::string name_;
  std::list<sink_ptr> sinks;
};

}
#endif //MATADOR_LOG_DOMAIN_HPP
