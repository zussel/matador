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

/**
 * @brief A file sink writeing log message to one file
 *
 * The log sink writes all log message to one single
 * file identified by a given path.
 *
 * Note because by there ist no limit the file grows infinitely.
 */
class OOS_LOGGER_API file_sink : public basic_file_sink
{
public:
  /**
   * Creates a file_sink with the given path.
   * If the the path doesn't exists, it is created.
   *
   * @param path The log file to write to
   */
  explicit file_sink(const std::string &path);

  /**
   * Creates a file_sink with the given path.
   * If the the path doesn't exists, it is created.
   *
   * @param path The log file to write to
   */
  explicit file_sink(const char *path);

  /**
   * Destroys the file_sink
   */
  ~file_sink() override;

  /**
   * Returns the path to the log file.
   *
   * @return The path to the log file
   */
  std::string path() const;

private:
  std::string path_;
};

/**
 * @brief Log sink writing to stdout
 *
 * This log sink writes all messages to stdout.
 */
class OOS_LOGGER_API stdout_sink : public basic_file_sink
{
public:
  stdout_sink();
  ~stdout_sink() override = default;

  /**
   * Do nothing on close
   */
  void close() override {}
};

/**
 * @brief Log sink writing to stderr
 *
 * This log sink writes all messages to stderr.
 */
class OOS_LOGGER_API stderr_sink : public basic_file_sink
{
public:
  stderr_sink();
  ~stderr_sink() override = default;

  /**
   * Do nothing on close
   */
  void close() override {}
};

}

#endif //MATADOR_FILE_SINK_HPP
