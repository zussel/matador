#ifndef MATADOR_LOG_MANAGER_HPP
#define MATADOR_LOG_MANAGER_HPP

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

#include "matador/utils/singleton.hpp"

#include "matador/logger/logger.hpp"
#include "matador/logger/log_sink.hpp"
#include "matador/logger/file_sink.hpp"
#include "matador/logger/rotating_file_sink.hpp"

#include <memory>
#include <list>

namespace matador {

/**
 * @brief Manages all log domains
 *
 * The log_manager class is a singleton and
 * manages all available log_domains
 *
 * There ist always a default log domain
 * with the name "default"
 * available for which sinks can be added
 * and loggers can be created.
 */
class OOS_LOGGER_API log_manager : public singleton<log_manager>
{
public:
  /**
   * Creates a logger with the given source name
   * for the default log domain
   *
   * @param source Name of the source
   * @return The created logger
   */
  logger create_logger(std::string source);

  /**
   * Creates a logger with the given source name
   * for the log domain identified by the given
   * log domain name.
   *
   * If the log domain with the given name doesn't exists
   * the domain ist created
   *
   * @param source Name of the source
   * @param domain_name The name of the log domain to execute to
   * @return The created logger
   */
  logger create_logger(std::string source, const std::string &domain_name);

  /**
   * Adds a log sink to the default log_domain
   *
   * @param sink Sink to add to the default log_domain
   */
  void add_sink(sink_ptr sink);

  /**
   * Adds a log sink to the log_domain with the given name.
   * If the log domain doesn't exists, it is automatically created.
   *
   * @param sink Sink to add
   * @param domain_name Name of the log domain
   */
  void add_sink(sink_ptr sink, const std::string &domain_name);

  /**
   * Clears all sinks from default log domain
   */
  void clear_all_sinks();

  /**
   * Clears all sinks from the log domain
   * with the given name
   *
   * @param domain_name Domain name to clear all sinks from
   */
  void clear_all_sinks(const std::string &domain_name);

  /**
   * Remove all log domains but the default log domain.
   * Clears all sinks from the default log domain.
   */
  void clear();

  /**
   * Sets the max default log level. Default
   * max leven is LVL_FATAL. All log domains
   * will start with this default max log range
   *
   * @param max_level max log level
   */
  static void max_default_log_level(log_level max_level);

  /**
   * Returns the default max log level
   *
   * @return The max log level
   */
  static log_level max_default_log_level();

  /**
   * Sets the default min log level. Default
   * min leven is LVL_INFO. All log domains
   * will start with this default max log range
   *
   * @param max_level min log level
   */
  static void min_default_log_level(log_level min_level);

  /**
   * Returns the default min log level
   *
   * @return The min log level
   */
  static log_level min_default_log_level();

  /// @cond MATADOR_DEV
  std::shared_ptr<log_domain> find_domain(const std::string &name);
  void log_default(log_level lvl, const std::string &source, const char *message);
  /// @endcond

protected:
  /// @cond MATADOR_DEV
  log_manager()
  {
    default_log_domain_ = log_domain_map.insert(std::make_pair("default", std::make_shared<log_domain>("default", default_log_level_range_))).first->second;
  }
  /// @endcond

private:
  std::shared_ptr<log_domain> acquire_domain(const std::string &name);

private:
  friend class singleton<log_manager>;

  std::shared_ptr<log_domain> default_log_domain_;

  std::map<std::string, std::shared_ptr<log_domain>> log_domain_map;

  static log_level_range default_log_level_range_;
};

/**
 * Shortcut to create a file log sink
 * with the given path. If the path doesn't
 * exists it is created.
 *
 * @param logfile Path to the logfile
 * @return A shared_ptr to the file_sink
 */
OOS_LOGGER_API std::shared_ptr<file_sink> create_file_sink(const std::string &logfile);

/**
 * Shortcut to create a stderr log sink.
 *
 * @return A shared_ptr to the stderr_sink
 */
OOS_LOGGER_API std::shared_ptr<stderr_sink> create_stderr_sink();

/**
 * Shortcut to create a stdout log sink.
 *
 * @return A shared_ptr to the stdout_sink
 */
OOS_LOGGER_API std::shared_ptr<stdout_sink> create_stdout_sink();

/**
 * Shortcut to create a rotating file log sink
 * with the given path, max log files and max
 * log file size. If the path doesn't
 * exists it is created.
 *
 * @param logfile Path to the log file
 * @param max_size Max log file size
 * @param file_count Max number of log files
 * @return A shared_ptr to the rotating_file_sink
 */
OOS_LOGGER_API std::shared_ptr<rotating_file_sink> create_rotating_file_sink(const std::string &logfile, size_t max_size, size_t file_count);

/**
 * Sets the default min log level.
 *
 * @param min_lvl Default min log level
 */
OOS_LOGGER_API void default_min_log_level(log_level min_lvl);

/**
 * Sets the default max log level.
 *
 * @param max_lvl Default max log level
 */
OOS_LOGGER_API void default_max_log_level(log_level max_lvl);

/**
 * Sets the domain min log level for the
 * domain with the given name.
 *
 * @param name Log domain name
 * @param min_lvl Default min log level
 */
OOS_LOGGER_API void domain_min_log_level(const std::string &name, log_level min_lvl);

/**
 * Sets the default max log level for the
 * domain with the given name.
 *
 * @param name Log domain name
 * @param max_lvl Default max log level
 */
OOS_LOGGER_API void domain_max_log_level(const std::string &name, log_level max_lvl);

/**
 * Adds a log sink to the default log domain
 *
 * @param sink The log sink to add
 */
OOS_LOGGER_API void add_log_sink(sink_ptr sink);

/**
 * Adds a log sink to the log domain
 * with the given name. If the domain
 * doesn't exists it is created.
 *
 * @param sink The log sink to add
 * @param domain The log domain name to add
 */
OOS_LOGGER_API void add_log_sink(sink_ptr sink, const std::string &domain);

/**
 * Removes all sinks from the
 * default domain
 */
OOS_LOGGER_API void clear_all_log_sinks();

/**
 * Removes all sinks from the log domain
 * with given domain name
 *
 * @param domain Domain name to clear all sinks
 */
OOS_LOGGER_API void clear_all_log_sinks(const std::string &domain);

/**
 * Creates a logger with the given source name
 * connected to the default log domain.
 *
 * @param source The name of the source
 * @return The logger instance
 */
OOS_LOGGER_API logger create_logger(std::string source);

/**
 * Creates a logger with the given source name
 * connected to the log domain with the given
 * name. If the domain doesn't exists it is created
 *
 * @param source The name of the source
 * @param domain The name of the log domain
 * @return The logger instance
 */
OOS_LOGGER_API logger create_logger(std::string source, const std::string &domain);

OOS_LOGGER_API void log_default(log_level lvl, const std::string &source, const char *message);

template<typename... ARGS>
void log(log_level lvl, const std::string &source, const char *what, ARGS const &... args)
{
  char message_buffer[16384];

#ifdef _MSC_VER
  sprintf_s(message_buffer, 912, what, args...);
#else
  sprintf(message_buffer, what, args...);
#endif

  log_default(lvl, source, message_buffer);
}

}

#endif //MATADOR_LOG_MANAGER_HPP
