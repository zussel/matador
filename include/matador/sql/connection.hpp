#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "matador/sql/result.hpp"
#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/statement.hpp"
#include "matador/sql/connection_info.hpp"
#include "matador/sql/connection_impl.hpp"
#include "matador/sql/row.hpp"
#include "matador/sql/field.hpp"
#include "matador/sql/basic_sql_logger.hpp"

#include "matador/utils/version.hpp"

#include <string>

namespace matador {

class basic_dialect;

/**
 * @brief The connection class represents a connection to a database.
 */
class connection
{
public:
  /**
   * @brief Creates an empty connection
   */
  connection() = default;

  /**
   * @brief Creates an empty connection
   */
  explicit connection(std::shared_ptr<basic_sql_logger> sqllogger);

  /**
   * @brief Creates a database connection from a connection string.
   *
   * @param dns The database connection string
   */
  explicit connection(const std::string &dns);
  
  /**
   * @brief Creates a database connection from a connection string.
   *
   * @param dns The database connection string
   * @param sql_logger The logger handler to write sql log messages to
   */
  connection(const std::string &dns, std::shared_ptr<basic_sql_logger> sql_logger);

  /**
   * Copies a given connection
   * 
   * @param x The connection to copy
   */
  connection(const connection &x);

  /**
   * Copy moves a given connection
   * 
   * @param x The connection to copy move
   */
  connection(connection &&x) noexcept;

  /**
   * Assigns from the given connection
   * 
   * @param x The connection to assign
   * @return The reference to the assigned connection
   */
  connection& operator=(const connection &x);

  /**
   * Assigns moves from the given connection
   * 
   * @param x The connection to assign move
   * @return The reference to the assigned connection
   */
  connection& operator=(connection &&x) noexcept;

  ~connection();

  /**
   * @brief Opens the sql for the given dns.
   *
   * Opens the sql. If sql couldn't be opened
   * an exception is thrown.
   */
  void connect(const std::string &dns);

  /**
   * @brief Opens the sql.
   *
   * Opens the sql. If sql couldn't be opened
   * an exception is thrown.
   */
  void connect();

  /**
   * Reconnect to the database. This means to close
   * and open the connection.
   */
  void reconnect();

  /**
   * @brief Returns true if sql is open.
   *
   * Returns true if sql is open
   *
   * @return True on open sql.
   */
  bool is_connected() const;

  /**
   * @brief Closes the sql.
   *
   * Closes the sql.
   */
  void disconnect();

  /**
   * @brief Executes the BEGIN TRANSACTION sql command
   */
  void begin();

  /**
   * @brief Executes the COMMIT TRANSACTION sql command
   */
  void commit();

  /**
   * @brief Executes the ROLLBACK TRANSACTION sql command
   */
  void rollback();

  /**
   * @brief Return the database type of the connection.
   *
   * Returns the database type of the connection which is
   * currently one of
   * - mssql
   * - mysql
   * - sqlite
   * - postgressql
   *
   * @return The database type string
   */
  std::string type() const;

  /**
   * @brief Return the version string of the current database type
   *
   * @return Version string of the current database type
   */
  version client_version() const;

  /**
   * @brief Return the version string of the current database type
   *
   * @return Version string of the current database type
   */
  version server_version() const;

  /**
   * @brief Execute a sql string statement with retrieving any result
   *
   * @param stmt The statement to be executed
   */
  void execute(const std::string &stmt)
  {
    logger_->on_execute(stmt);
    std::unique_ptr<detail::result_impl> res(impl_->execute(stmt));
  }

  /**
   * @brief returns true if a table with given name exists.
   *
   * @param table_name Name of table to be checked
   * @return True if table exists.
   */
  bool exists(const std::string &table_name) const;

  /**
   * @brief Retrieve a field description list of a table
   * @param table The name of the requested table
   * @return A list of fields
   */
  std::vector<field> describe(const std::string &table) const;

  /**
   * @brief Get the underlying sql dialect object.
   *
   * @return Sql dialect object.
   */
  basic_dialect* dialect();

  /**
   * @brief Returns true if connection is valid
   *
   * Returns true if connection is valid, i.e. the
   * database connection string is set correctly
   *
   * @return True if connection is valid
   */
  bool is_valid() const;

  /**
   * Enable console log of sql statements
   */
  void enable_log();

  /**
   * Disable console log of sql statements
   */
  void disable_log();

  /**
   * Returns true if logging is enabled.
   *
   * @return True if logging is enabled
   */
  bool is_log_enabled() const;

private:
  template < class T >
  friend class query;

  void initialize_connection_info(const std::string &dns);
  template <class Type>
  void prepare_prototype_row(Type &/*prototype*/, const std::string &/*table_name*/) {}
  void prepare_prototype_row(row &prototype, const std::string &table_name);

  template < class Type >
  result<Type> execute(const sql &stmt, const std::string &table_name, const Type &prototype)
  {
    // get column descriptions
    prepare_prototype_row(prototype, table_name);
    auto sql_stmt = dialect()->direct(stmt);
    logger_->on_execute(sql_stmt);
    return result<Type>(impl_->execute(sql_stmt), prototype);
  }

  template < class Type >
  statement<Type> prepare(const matador::sql &sql, const std::string &table_name, const Type &prototype)
  {
    prepare_prototype_row(prototype, table_name);
    auto stmt = statement<Type>(impl_->prepare(dialect()->prepare(sql)), prototype, logger_);
    if (is_log_enabled()) {
      stmt.enable_log();
    }
    return stmt;
  }

private:
  static connection_impl* create_connection(const std::string &type) ;
  void init_from_foreign_connection(const connection &foreign_connection);

  void log_token(detail::token::t_token tok);

private:
  connection_info connection_info_{};
  std::unique_ptr<connection_impl> impl_;

  std::shared_ptr<basic_sql_logger> logger_ = std::make_shared<null_sql_logger>();
};

}
#endif /* CONNECTION_HPP */
