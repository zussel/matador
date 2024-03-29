#ifndef MATADOR_LOG_DOMAIN_HPP
#define MATADOR_LOG_DOMAIN_HPP

#include "matador/logger/export.hpp"

#include "matador/logger/log_sink.hpp"
#include "matador/logger/log_level.hpp"

#include <string>
#include <list>
#include <map>
#include <mutex>

namespace matador {

/**
 * @brief Connection to a set of log sinks
 *
 * A log domain is the connection point between
 * a set of log sinks and the logger objects
 * in the user code.
 *
 * A domain consists of a unique name and a
 * list of sinks
 */
class OOS_LOGGER_API log_domain
{
public:
  /**
   * The timeformat for each log line
   */
  static constexpr const char *TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S.%f";

  /**
   * Creates a log_domain with the given name
   * and the given log range
   *
   * @param name The name of the log domain
   * @param log_range The log range of this domain
   */
  log_domain(std::string name, log_level_range log_range);

  /**
   * Returns the name of the domain
   *
   * @return The name of the domain
   */
  std::string name() const;

  /**
   * Sets the max log level. Default
   * max leven is LVL_FATAL
   *
   * @param max_level max log level
   */
  void max_log_level(log_level max_level);

  /**
   * Returns the max log level
   *
   * @return The max log level
   */
  log_level max_log_level() const;

  /**
   * Sets the min log level. Default
   * min leven is LVL_INFO
   *
   * @param min_level min log level
   */
  void min_log_level(log_level min_level);

  /**
   * Returns the min log level
   *
   * @return The min log level
   */
  log_level min_log_level() const;

  /**
   * Add a sink to the domain.
   *
   * The sink must be packed into a std::shared_ptr
   * because it can be shared among other domains
   *
   * @param sink The sink to add
   */
  void add_sink(sink_ptr sink);

  /**
   * Logs the given message for the given source and log level
   * to this log domain.
   *
   * @param lvl Log level
   * @param source Source of the log message
   * @param message Message to log
   */
  void log(log_level lvl, const std::string &source, const char *message);

  /**
   * Clears the list of log sinks
   */
  void clear();

private:
  void get_time_stamp(char* timestamp_buffer);

private:
  static std::map<log_level, std::string> level_strings;

  std::string name_;
  std::list<sink_ptr> sinks;

  log_level_range log_level_range_;

  std::mutex mutex_;
};

}
#endif //MATADOR_LOG_DOMAIN_HPP
