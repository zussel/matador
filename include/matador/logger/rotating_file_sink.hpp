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
 * This log sink provides a possibility to
 * rotate several log files if the current
 * log file reaches the maximum size.
 *
 * The user can define the maximum number of log files
 * and the maximum size of the current log file
 *
 * The name of the current log file is defined within the
 * given logfile path. Each rotated (moved) log file gets
 * an incremented number extension right before the
 * file extension, e.g.:
 *
 * Log file name is 'log.txt' the first rotated log file
 * is named 'log-1.txt' and so on until the maximum
 * number of log files is reached. Then it starts from
 * the beginning.
 * Keep in mind that the log file to which is currently
 * written to is always named like the file name
 * given within the path.
 */
class OOS_LOGGER_API rotating_file_sink : public log_sink
{
public:
  /**
   * Creates a rotating log file sink within the given path
   * with the given maximum number of rotating log files where
   * each file size is never greater than the given max file
   * size.
   *
   * @param path Path of the log file
   * @param max_size Max log file size
   * @param file_count Max log file count
   */
  rotating_file_sink(const std::string& path, size_t max_size, size_t file_count);

  /**
   * Write the message to current log file. If the
   * actual size exceeds the file size limit the log files
   * are rotated.
   *
   * @param message Log message to write
   * @param size The size of the log message
   */
  void write(const char *message, size_t size) override;

  /**
   * Close all open log files
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
