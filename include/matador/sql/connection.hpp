#ifndef QUERY_CONNECTION_HPP
#define QUERY_CONNECTION_HPP

#include "matador/sql/connection_info.hpp"
#include "matador/sql/connection_impl.hpp"
#include "matador/sql/dialect.hpp"
#include "matador/sql/query.hpp"
#include "matador/sql/query_context.hpp"
#include "matador/sql/query_result.hpp"
#include "matador/sql/record.hpp"
#include "matador/sql/statement.hpp"

#include <string>

namespace matador::sql {
class schema;

/**
 * @brief The connection class represents a connection to a database.
 */
class connection final {
public:
  /**
   * @brief Creates a database connection from a connection info data.
   *
   * @param info The database connection info data
   * @param sqllogger The logging handler
   */
  explicit connection(connection_info info,
                      const std::shared_ptr<basic_sql_logger> &sqllogger = std::make_shared<null_sql_logger>());
  /**
   * @brief Creates a database connection from a connection string.
   *
   * @param dns The database connection string
   * @param sqllogger The logging handler
   */
  explicit connection(const std::string &dns,
                      const std::shared_ptr<basic_sql_logger> &sqllogger = std::make_shared<null_sql_logger>());
  /**
   * Copies a given connection
   *
   * @param x The connection to copy
   */
  connection(const connection &x);
  /**
   * Assigns from the given connection
   *
   * @param x The connection to assign
   * @return The reference to the assigned connection
   */
  connection &operator=(const connection &x);
  /**
   * Copy moves a given connection
   *
   * @param x The connection to copy move
   */
  connection(connection &&x) noexcept = default;
  /**
   * Assigns moves from the given connection
   *
   * @param x The connection to assign move
   * @return The reference to the assigned connection
   */
  connection& operator=(connection &&x) noexcept;

  ~connection();

  /**
   * @brief Opens the database connection for the given dns.
   *
   * Opens the database connection. If database connection
   * couldn't be opened an exception is thrown.
   */
  void open() const;
  /**
   * @brief Closes the database connection.
   *
   * Closes the database connection.
   */
  void close() const;
  /**
   * @brief Returns true if database connection is open.
   *
   * Returns true if database connection is open
   *
   * @return True on open database connection.
   */
  [[nodiscard]] bool is_open() const;

  /**
   * Returns the connection info data of the
   * current database connection.
   *
   * @return Returns the connection info data
   */
  [[nodiscard]] const connection_info &info() const;
  /**
   * @brief Return the database type of the connection.
   *
   * Returns the database type of the connection which is
   * currently one of
   * - mssql
   * - mysql
   * - sqlite
   * - postgres
   *
   * @return The database type string
   */
  [[nodiscard]] std::string type() const;

  /**
   * @brief Starts a transaction by calling the
   * underlying database backend transaction begin
   * statement.
   */
  void begin() const;

  /**
   * @brief Commits a transaction by calling the
   * underlying database backend transaction commit
   * statement.
   */
  void commit() const;

  /**
   * @brief Rollback a transaction by calling the
   * underlying database backend transaction rollback/abort
   * statement.
   */
  void rollback() const;

  [[nodiscard]] std::vector<sql::column_definition> describe(const std::string &table_name) const;
  [[nodiscard]] bool exists(const std::string &schema_name, const std::string &table_name) const;
  [[nodiscard]] bool exists(const std::string &table_name) const;
  [[nodiscard]] sql::query query(const sql::schema &schema) const;
  [[nodiscard]] query_result<record> fetch(const query_context &q) const;
  [[nodiscard]] size_t execute(const std::string &sql) const;

  statement prepare(query_context &&query) const;

  [[nodiscard]] const class dialect &dialect() const;

private:
  [[nodiscard]] std::unique_ptr<query_result_impl> fetch(const std::string &sql) const;

private:
  friend class query_select;
  friend class session;

  connection_info connection_info_;
  std::unique_ptr<connection_impl> connection_;
  const class dialect &dialect_;
  std::shared_ptr<basic_sql_logger> logger_ = std::make_shared<null_sql_logger>();
};
}

#endif //QUERY_CONNECTION_HPP
