#ifndef MATADOR_SQL_LOGGER_HPP
#define MATADOR_SQL_LOGGER_HPP

//#include "matador/sql/export.hpp"

#include "matador/sql/basic_sql_logger.hpp"
#include "matador/logger/logger.hpp"

namespace matador {

/**
 * The sql logger uses the matador
 * logging mechanism with two sink. A rotating
 * file sink besides a stdout sink.
 */
//class OOS_SQL_API sql_logger : public basic_sql_logger
class sql_logger : public basic_sql_logger
{
public:
  /**
   * Creates a sql logger using the
   * matador logging mechanism. Internally a
   * rotating file sink is used. The path is
   * "log/sql.log" with a max file size of 1mb
   * and maximum log file count of 5.
   *
   * In addition a stdout log sink is used.
   */
  sql_logger();

  /**
   * Creates a sql logger using the
   * matador logging mechanism. Internally a
   * rotating file sink is used. The path is
   * "log/sql.log" with the given max file size
   * and the given maximum log file count
   *
   * In addition a stdout log sink is used.
   *
   * @param max_size Max file size
   * @param file_count Max log file number
   */
  sql_logger(size_t max_size, size_t file_count);

  /**
   * No log on connect
   */
  void on_connect() override { }

  /**
   * No log on disconnect
   */
  void on_close() override { }

  /**
   * Logs the statement when it is executed
   *
   * @param stmt Statement to log
   */
  void on_execute(const std::string &stmt) override;

  /**
   * Logs the statement when it is prepared
   *
   * @param stmt Statement to log
   */
  void on_prepare(const std::string &stmt) override;

private:
  matador::logger LOG;
};

}
#endif //MATADOR_SQL_LOGGER_HPP
