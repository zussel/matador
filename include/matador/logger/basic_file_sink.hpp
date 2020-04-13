#ifndef MATADOR_BASIC_FILE_SINK_HPP
#define MATADOR_BASIC_FILE_SINK_HPP

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

#include "matador/logger/log_sink.hpp"

#include <cstdio>

namespace matador {

class OOS_LOGGER_API basic_file_sink : public log_sink
{
protected:
  basic_file_sink() = default;
  explicit basic_file_sink(FILE *f);

public:
  void write(const char *message, size_t size) override;

  void close() override;

protected:
  FILE *stream = nullptr;
};

}
#endif //MATADOR_BASIC_FILE_SINK_HPP
