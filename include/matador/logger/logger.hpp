#ifndef MATADOR_LOGGER_HPP
#define MATADOR_LOGGER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
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

class OOS_LOGGER_API logger
{
public:

  logger(std::string source, std::shared_ptr<log_domain> log_domain);

  template<typename ... ARGS>
  void info(const std::string &what, ARGS const &... args) { info(what.c_str(), args...); }
  template<typename ... ARGS>
  void info(const char *what, ARGS const &... args) { log(log_level::LVL_INFO, what, args...); }
  template<typename ... ARGS>
  void debug(const std::string &what, ARGS const &... args) { debug(what.c_str(), args...); }
  template<typename ... ARGS>
  void debug(const char *what, ARGS const &... args) { log(log_level::LVL_DEBUG, what, args...); }
  template<typename ... ARGS>
  void warn(const std::string &what, ARGS const &... args) { warn(what.c_str(), args...); }
  template<typename ... ARGS>
  void warn(const char *what, ARGS const &... args) { log(log_level::LVL_WARN, what, args...); }
  template<typename ... ARGS>
  void error(const std::string &what, ARGS const &... args) { error(what.c_str(), args...); }
  template<typename ... ARGS>
  void error(const char *what, ARGS const &... args) { log(log_level::LVL_ERROR, what, args...); }
  template<typename ... ARGS>
  void trace(const std::string &what, ARGS const &... args) { trace(what.c_str(), args...); }
  template<typename ... ARGS>
  void trace(const char *what, ARGS const &... args) { log(log_level::LVL_TRACE, what, args...); }
  template<typename ... ARGS>
  void log(log_level lvl, const char *what, ARGS const &... args);

  std::string source() const;
  std::string domain() const;

private:
  std::string source_;
  std::shared_ptr<log_domain> logger_domain_;
};

template<typename... ARGS>
void logger::log(log_level lvl, const char *what, ARGS const &... args)
{
  char message_buffer[912];

  sprintf(message_buffer, what, args...);

  logger_domain_->log(lvl, source_, message_buffer);
}

}

#endif //MATADOR_LOGGER_HPP
