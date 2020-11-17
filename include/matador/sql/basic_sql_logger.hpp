#ifndef MATADOR_BASIC_SQL_LOGGER_HPP
#define MATADOR_BASIC_SQL_LOGGER_HPP

#include <string>

namespace matador {

/**
 * @brief Base class for sql logging
 *
 * Thsi class acts as a base class to
 * implement a concrete logger for sql
 * statements.
 *
 * It provides interfaces to handle
 * the establishing and closing of
 * a database connection as well as
 * when an sql statement is about to
 * execute or going to prepared.
 */
class basic_sql_logger
{
public:
  /**
   * Is called when a connection to a database is
   * going to be established
   */
  virtual void on_connect() = 0;

  /**
   * Is called when a connection is going to be closed
   */
  virtual void on_close() = 0;

  /**
   * Is called when a sql statement is going to
   * be executed
   *
   * @param stmt SQL statement to be executed
   */
  virtual void on_execute(const std::string &stmt) = 0;

  /**
   * Is called when a sql statement is going to
   * be prepared
   *
   * @param stmt SQL statement to be prepared
   */
  virtual void on_prepare(const std::string &stmt) = 0;
};

/**
 * Implements the basic_sql_logger to do no
 * logging at all.
 * This is used as the default logger for all
 * connections and statements.
 */
class null_sql_logger : public basic_sql_logger
{
public:
  /**
   * No logging on establishing a connection.
   */
  void on_connect() override { }

  /**
   * No logging on closing a connection.
   */
  void on_close() override { }

  /**
   * No logging on executing a statement.
   */
  void on_execute(const std::string &) override { }

  /**
   * No logging on preparing a statement.
   */
  void on_prepare(const std::string &) override { }
};

}

#endif //MATADOR_BASIC_SQL_LOGGER_HPP
