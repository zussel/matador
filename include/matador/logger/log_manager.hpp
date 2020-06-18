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

OOS_LOGGER_API std::shared_ptr<file_sink> create_file_sink(const std::string &logfile);
OOS_LOGGER_API std::shared_ptr<stderr_sink> create_stderr_sink();
OOS_LOGGER_API std::shared_ptr<stdout_sink> create_stdout_sink();
OOS_LOGGER_API std::shared_ptr<rotating_file_sink> create_rotating_file_sink(const std::string &logfile, size_t max_size, size_t file_count);

OOS_LOGGER_API void add_log_sink(sink_ptr sink);
OOS_LOGGER_API void add_log_sink(sink_ptr sink, const std::string &domain);

OOS_LOGGER_API logger create_logger(std::string source);
OOS_LOGGER_API logger create_logger(std::string source, const std::string &domain);

}

#endif //MATADOR_LOG_MANAGER_HPP
