#ifndef MATADOR_FILE_SINK_HPP
#define MATADOR_FILE_SINK_HPP

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

#include "matador/logger/basic_file_sink.hpp"

#include <string>
#include <stdexcept>

namespace matador {

class OOS_LOGGER_API file_sink : public basic_file_sink
{
public:
  explicit file_sink(const std::string &path);
  explicit file_sink(const char *path);

  ~file_sink() override;

  std::string path() const;

private:
  std::string path_;
};

class OOS_LOGGER_API stdout_sink : public basic_file_sink
{
public:
  stdout_sink();
  ~stdout_sink() override = default;
};

class OOS_LOGGER_API stderr_sink : public basic_file_sink
{
public:
  stderr_sink();
  ~stderr_sink() override = default;
};

}

#endif //MATADOR_FILE_SINK_HPP
