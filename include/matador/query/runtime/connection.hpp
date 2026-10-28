#ifndef QUERY_CONNECTION_HPP
#define QUERY_CONNECTION_HPP

#include "matador/query/runtime/abstract_sql_logger.hpp"
#include "matador/query/runtime/executor.hpp"
#include "matador/query/runtime/connection_info.hpp"
#include "matador/query/runtime/execute_result.hpp"
#include "matador/query/resolver_service.hpp"
#include "matador/query/runtime/statement.hpp"

#include <memory>
#include <string>
#include <vector>

namespace matador::query {
class connection_impl;

/**
 * @brief The connection class represents a connection to a database.
 */
class connection final : public executor {
public:
  using logger_ptr = std::shared_ptr<abstract_sql_logger>;
  /**
   * @brief Creates a database connection from connection info data.
   *
   * @param info The database connection info data
   * @param sql_logger The logging handler
   */
  explicit connection(const connection_info& info, const logger_ptr& sql_logger = null_logger);
  explicit connection(const connection_info& info, std::shared_ptr<resolver_service> resolver, const logger_ptr& sql_logger = null_logger);
  /**
   * @brief Creates a database connection from a connection string.
   *
   * @param dns The database connection string
   * @param sql_logger The logging handler
   */
  explicit connection(const std::string &dns, const logger_ptr &sql_logger = null_logger);
  explicit connection(const std::string &dns, const std::shared_ptr<resolver_service>& resolver, const logger_ptr &sql_logger = null_logger);
  /**
   * Copies a given connection
   *
   * @param x The connection to copy
   */
  connection(const connection &x) = delete;
  /**
   * Assigns from the given connection
   *
   * @param x The connection to assign
   * @return The reference to the assigned connection
   */
  connection &operator=(const connection &x) = delete;
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
  connection& operator=(connection &&x) noexcept = default;

  ~connection() override;

  /**
   * @brief Opens the database connection for the given dns.
   *
   * Opens the database connection. If the database connection
   * couldn't be opened, an exception is thrown.
   */
  [[nodiscard]] result<void, error> open() const;
  /**
   * @brief Closes the database connection.
   *
   * Closes the database connection.
   */
  [[nodiscard]] result<void, error> close() const;
  /**
   * @brief Returns true if the database connection is open.
   *
   * Returns true if the database connection is open
   *
   * @return True on an open database connection.
   */
  [[nodiscard]] result<bool, error> is_open() const;

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
   * Returns the database type of the connection, which is
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
   * underlying database backends a transaction begin
   * statement.
   */
  [[nodiscard]] result<void, error> begin() const;

  /**
   * @brief Commits a transaction by calling the
   * underlying database backends a transaction commit
   * statement.
   */
  [[nodiscard]] result<void, error> commit() const;

  /**
   * @brief Roll back a transaction by calling the
   * underlying database backends transaction rollback or abort
   * statement.
   */
  [[nodiscard]] result<void, error> rollback() const;

  [[nodiscard]] result<std::vector<column>, error> describe(const std::string &table_name) const;
  [[nodiscard]] result<bool, error> exists(const std::string &schema_name, const std::string &table_name) const;
  [[nodiscard]] result<bool, error> exists(const std::string &table_name) const;


  [[nodiscard]] result<bool, error> sequence_exists(const std::string &sequence_name) const;

  [[nodiscard]] result<std::unique_ptr<query_result_impl>, error> fetch(const query_context &ctx) const override;
  [[nodiscard]] result<execute_result, error> execute(const query_context &ctx) const override;
  [[nodiscard]] result<statement, error> prepare(const query_context &ctx) const override;
  [[nodiscard]] std::string str( const query_context& ctx ) const override;

  [[nodiscard]] const class dialect &dialect() const override;
  [[nodiscard]] std::shared_ptr<resolver_service> resolver() const override;

private:
  [[nodiscard]] result<std::unique_ptr<statement_impl>, error> perform_prepare(const query_context &ctx) const;

  struct connection_deleter {
    void operator()(connection_impl *impl) const noexcept;
  };

  using connection_ptr = std::unique_ptr<connection_impl, connection_deleter>;

private:
  friend class fetchable_query;
  friend class session;
  friend class statement_cache;

  connection_ptr connection_;
  logger_ptr logger_ = null_logger;
  std::shared_ptr<resolver_service> resolver_service_ = std::make_shared<resolver_service>();
};
}

#endif //QUERY_CONNECTION_HPP
