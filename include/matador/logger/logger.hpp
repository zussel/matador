#ifndef MATADOR_LOGGER_HPP
#define MATADOR_LOGGER_HPP

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

#include "matador/logger/log_level.hpp"
#include "matador/logger/log_domain.hpp"

#include <string>
#include <map>
#include <list>
#include <memory>

namespace matador {

/**
 * @brief logger to write log messages to log domains
 *
 * This class is used to write log messages to a connected
 * log domain (@sa log_domain).
 * Everywhere a logger is needed, it can be instantiated with
 * @code
 * matador::create_logger(source name)
 * @endcode
 *
 * The interface provides methods to log to each relevant
 * log level (@sa log_level)
 *
 * The message format syntax is like the printf syntax.
 * If the message string contains placeholder (beginning with %)
 * an argument is expected to be part of the argument list of the
 * calling method.
 *
 * All log messages are written through the internal
 * log_domain object to the sinks.
 */
class OOS_LOGGER_API logger
{
public:

  /**
   * Create a logger with a given source name connected
   * to the given log_domain
   *
   * @param source The name of the source
   * @param log_domain The log_domain containing the log sinks
   */
  logger(std::string source, std::shared_ptr<log_domain> log_domain);

  /**
   * Writes a log message string with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void fatal(const std::string &what, ARGS const &... args) { fatal(what.c_str(), args...); }

  /**
   * Writes a log message represented by a char pointer with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void fatal(const char *what, ARGS const &... args) { log(log_level::LVL_FATAL, what, args...); }

  /**
   * Writes a log message string with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void error(const std::string &what, ARGS const &... args) { error(what.c_str(), args...); }

  /**
   * Writes a log message represented by a char pointer with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void error(const char *what, ARGS const &... args) { log(log_level::LVL_ERROR, what, args...); }

  /**
   * Writes a log message string with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void warn(const std::string &what, ARGS const &... args) { warn(what.c_str(), args...); }

  /**
   * Writes a log message represented by a char pointer with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void warn(const char *what, ARGS const &... args) { log(log_level::LVL_WARN, what, args...); }

  /**
   * Writes a log message string with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void info(const std::string &what, ARGS const &... args) { info(what.c_str(), args...); }

  /**
   * Writes a log message represented by a char pointer with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void info(const char *what, ARGS const &... args) { log(log_level::LVL_INFO, what, args...); }

  /**
   * Writes a log message string with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void debug(const std::string &what, ARGS const &... args) { debug(what.c_str(), args...); }

  /**
   * Writes a log message represented by a char pointer with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void debug(const char *what, ARGS const &... args) { log(log_level::LVL_DEBUG, what, args...); }

  /**
   * Writes a log message string with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void trace(const std::string &what, ARGS const &... args) { trace(what.c_str(), args...); }

  /**
   * Writes a log message represented by a char pointer with log level LVL_FATAL
   * to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void trace(const char *what, ARGS const &... args) { log(log_level::LVL_TRACE, what, args...); }

  /**
   * Writes a log message represented by a char pointer
   * with the given log level to the connected log_domain.
   *
   * @tparam ARGS Type of the arguments to replaced for the message placeholder
   * @param lvl The log level
   * @param what The message to log
   * @param args The arguments to be replaced in the message
   */
  template<typename ... ARGS>
  void log(log_level lvl, const char *what, ARGS const &... args);

  /**
   * Writes a log message represented by a char pointer
   * with the given log level to the connected log_domain.
   *
   * @param lvl The log level
   * @param what The message to log
   */
  void log(log_level lvl, const char *what);

  /**
   * Returns the name of the source the logger represents
   *
   * @return Represented log source name
   */
  std::string source() const;

  /**
   * Returns the name of the connected log domain
   *
   * @return The name of the log domain
   */
  std::string domain() const;

private:
  std::string source_;
  std::shared_ptr<log_domain> logger_domain_;
};

template<typename... ARGS>
void logger::log(log_level lvl, const char *what, ARGS const &... args)
{
  char message_buffer[16384];

#ifdef _MSC_VER
  sprintf_s(message_buffer, 16384, what, args...);
#else
  sprintf(message_buffer, what, args...);
#endif

  logger_domain_->log(lvl, source_, message_buffer);
}

/*

 template<typename T>
decltype(auto) myForward(T&& t)
{
    return t;
}

template<>
decltype(auto) myForward(std::string& t)
{
    return t.c_str();
}

template<>
decltype(auto) myForward(std::string&& t)
{
    return t.c_str();
}

template<typename... Args>
static void log(const char* pszFmt, Args&&... args)
{
    doSomething(pszFmt, myForward<Args>(std::forward<Args>(args))...);
}

 */
}

#endif //MATADOR_LOGGER_HPP
