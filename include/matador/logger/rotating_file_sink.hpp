#ifndef MATADOR_ROTATING_FILE_SINK_HPP
#define MATADOR_ROTATING_FILE_SINK_HPP

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
#include "matador/utils/file.hpp"

#include <vector>

namespace matador {

class OOS_LOGGER_API rotating_file_sink : public log_sink
{
public:
  rotating_file_sink(const std::string& path, size_t max_size, size_t file_count);

  void write(const char *message, size_t size) override;

  void close() override;

private:
  std::string calculate_filename(size_t fileno);

  void rotate();
  void prepare(const std::string &path);

private:
  file logfile_;
  std::string path_;
  std::string base_path_;
  std::string extension_;
  size_t max_size_ = 0;
  size_t current_size_ = 0;
  size_t current_file_no_ = 0;
  size_t file_count_ = 0;
};

}
#endif //MATADOR_ROTATING_FILE_SINK_HPP
