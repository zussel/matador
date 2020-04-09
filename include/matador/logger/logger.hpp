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

#include "matador/utils/time.hpp"

#include <string>
#include <map>

namespace matador {

class log_manager;

class OOS_LOGGER_API logger
{
public:
  enum level
  {
    LVL_INFO, LVL_WARN, LVL_DEBUG, LVL_ERROR, LVL_TRACE
  };

  explicit logger(const char *source, log_manager &manager);

  template<typename ... ARGS>
  void info(const std::string &what, ARGS const &... args) { info(what.c_str(), args...); }
  template<typename ... ARGS>
  void info(const char *what, ARGS const &... args) { log(LVL_INFO, what, args...); }
  template<typename ... ARGS>
  void debug(const std::string &what, ARGS const &... args) { debug(what.c_str(), args...); }
  template<typename ... ARGS>
  void debug(const char *what, ARGS const &... args) { log(LVL_DEBUG, what, args...); }
  template<typename ... ARGS>
  void warn(const std::string &what, ARGS const &... args) { warn(what.c_str(), args...); }
  template<typename ... ARGS>
  void warn(const char *what, ARGS const &... args) { log(LVL_WARN, what, args...); }
  template<typename ... ARGS>
  void error(const std::string &what, ARGS const &... args) { error(what.c_str(), args...); }
  template<typename ... ARGS>
  void error(const char *what, ARGS const &... args) { log(LVL_ERROR, what, args...); }
  template<typename ... ARGS>
  void trace(const std::string &what, ARGS const &... args) { trace(what.c_str(), args...); }
  template<typename ... ARGS>
  void trace(const char *what, ARGS const &... args) { log(LVL_TRACE, what, args...); }
  template<typename ... ARGS>
  void log(level lvl, const char *what, ARGS const &... args);

private:

  void log(const char *timestamp, const char *level, const char *message);

  const char *gettimestamp(char *buffer, size_t size) const;

  static constexpr const char *TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S.%f";

  static std::map<level, std::string> level_strings;

  const char *source_ = nullptr;
  log_manager &manager_;
};

template<typename... ARGS>
void logger::log(logger::level lvl, const char *what, ARGS const &... args)
{
  char timestamp_buffer [80];

  const char *timestamp = gettimestamp(timestamp_buffer, 80);

  char message_buffer[912];

  sprintf(message_buffer, what, args...);

  log(timestamp, level_strings[lvl].c_str(), message_buffer);
}

}

#endif //MATADOR_LOGGER_HPP
