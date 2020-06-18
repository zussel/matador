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

/**
 * @brief A rotating log file sink
 *
 * This rotating log file sink rotates the log files within
 * given number of maximum log files and a given maximum
 * file size.
 *
 * Once the max file size is reached the next log file is
 * created or overwritten.
 * Once the maximum number of log files are reached the first
 * log file is used to write messages on.
 */
class OOS_LOGGER_API rotating_file_sink : public log_sink
{
public:
  /**
   * Creates a rotating log file sink where the log files
   * reside in the given path. Also the max file size and
   * max number of log files for rotation must be given.
   *
   * @param path Path of the log file
   * @param max_size Max size of one log file
   * @param file_count Max number of log files
   */
  rotating_file_sink(const std::string& path, size_t max_size, size_t file_count);

  /**
   * Writes the message to the current log file and if
   * necessary sink rotates to the next log file.
   *
   * @param message The message to log
   * @param size The size of the message
   */
  void write(const char *message, size_t size) override;

  /**
   * Closes the current log file stream
   */
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
