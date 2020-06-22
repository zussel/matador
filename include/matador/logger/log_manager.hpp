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
   * @param domain_name The name of the log domain to connect to
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
   * Remove all log domains but the default log domain.
   * Clears all sinks from the default log domain.
   */
  void clear();

protected:
  /// @cond MATADOR_DEV
  log_manager()
  {
    default_log_domain_ = log_domain_map.insert(std::make_pair("default", std::make_shared<log_domain>("default"))).first->second;
  }
  /// @endcond

private:
  std::shared_ptr<log_domain> acquire_domain(const std::string &name);

private:
  friend class singleton<log_manager>;

  std::shared_ptr<log_domain> default_log_domain_;

  std::map<std::string, std::shared_ptr<log_domain>> log_domain_map;
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

}

#endif //MATADOR_LOG_MANAGER_HPP
