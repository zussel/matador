#ifndef MATADOR_FILE_SINK_HPP
#define MATADOR_FILE_SINK_HPP

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
};

}

#endif //MATADOR_FILE_SINK_HPP
