#ifndef MATADOR_LOG_SINK_HPP
#define MATADOR_LOG_SINK_HPP

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

#include <cstddef>
#include <memory>

namespace matador {

class OOS_LOGGER_API log_sink
{
public:
  virtual ~log_sink() = default;

  virtual void write(const char *message, std::size_t size) = 0;

  virtual void close() = 0;
};

using sink_ptr = std::shared_ptr<log_sink>;

}

#endif //MATADOR_LOG_SINK_HPP
