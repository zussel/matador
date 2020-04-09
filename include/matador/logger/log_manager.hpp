#ifndef MATADOR_LOG_MANAGER_HPP
#define MATADOR_LOG_MANAGER_HPP

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

#include "matador/logger/logger.hpp"
#include "matador/logger/log_sink.hpp"

#include <memory>
#include <list>

namespace matador {

class OOS_LOGGER_API log_manager
{
public:
  logger create_logger(const char *source);

  void add(std::unique_ptr<log_sink> sink);

  void log(const char *msg, size_t size);
private:
  std::list<std::unique_ptr<log_sink>> sinks;
};

}

#endif //MATADOR_LOG_MANAGER_HPP
