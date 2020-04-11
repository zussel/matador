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

#include "matador/utils/singleton.hpp"

#include "matador/logger/logger.hpp"
#include "matador/logger/log_sink.hpp"

#include <memory>
#include <list>

namespace matador {

class OOS_LOGGER_API log_manager : public singleton<log_manager>
{
public:
  logger create_logger(std::string source);
  logger create_logger(std::string source, const std::string &domain_name);

  void add_sink(sink_ptr sink);
  void add_sink(sink_ptr sink, const std::string &domain_name);

protected:
  log_manager()
  {
    default_log_domain_ = log_domain_map.insert(std::make_pair("default", std::make_shared<logger_domain>())).first->second;
  }

private:
  std::shared_ptr<logger_domain> acquire_domain(const std::string &name);

private:
  friend class singleton<log_manager>;

  std::shared_ptr<logger_domain> default_log_domain_;

  std::map<std::string, std::shared_ptr<logger_domain>> log_domain_map;
};

void add_log_sink(sink_ptr sink);
void add_log_sink(sink_ptr sink, const std::string &domain);

logger create_logger(std::string source);
logger create_logger(std::string source, const std::string &domain);

}

#endif //MATADOR_LOG_MANAGER_HPP
