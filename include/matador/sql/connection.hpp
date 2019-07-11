#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/result.hpp"
#include "matador/sql/statement.hpp"
#include "matador/sql/connection_impl.hpp"
#include "row.hpp"
#include "field.hpp"

#include <string>

namespace matador {

class basic_dialect;

/**
 * @brief The connection class represents a connection to a database.
 */
class OOS_SQL_API connection
{
public:
  /**
   * @brief Creates an empty connection
   */
  connection();

  /**
   * @brief Creates a database connection from a connection string.
   *
   * @param dns The database connection string
   */
  explicit connection(const std::string &dns);
  
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
  void open(const std::string &dns);

  /**
   * @brief Opens the sql.
   *
   * Opens the sql. If sql couldn't be opened
   * an exception is thrown.
   */
  void open();

  /**
   * @brief Returns true if sql is open.
   *
   * Returns true if sql is open
   *
   * @return True on open sql.
   */
  bool is_open() const;

  /**
   * @brief Closes the sql.
   *
   * Closes the sql.
   */
  void close();

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
   *
   * @return The database type string
   */
  std::string type() const;
  /**
   * @brief Return the version string of the current database type
   *
   * @return Version string of the current database type
   */
  std::string version() const;

  /**
   * @brief Execute a sql string statement with retrieving any result
   *
   * @param stmt The statement to be executed
   */
  void execute(const std::string &stmt)
  {
    std::unique_ptr<detail::result_impl> res(impl_->execute(stmt));
  }

  /**
   * @brief returns true if a table with given name exists.
   *
   * @param tablename Name of table to be checked
   * @return True if table exists.
   */
  bool exists(const std::string &tablename) const;

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

private:
  template < class T >
  friend class query;

  void prepare_prototype_row(row &prototype, const std::string &tablename);

  template < class T >
  result<T> execute(const sql &stmt, const std::string &tablename, row prototype, typename std::enable_if< std::is_same<T, row>::value >::type* = 0)
  {
    // get column descriptions
    prepare_prototype_row(prototype, tablename);
    std::cout << impl_->dialect()->direct(stmt) << "\n";
    return result<T>(impl_->execute(stmt), prototype);
  }

  /**
   * @brief Execute a sql object representing a statement
   *
   * @tparam T The entity type of the query
   * @param stmt The statement to be executed
   * @return A result object
   */
  template < class T >
  result<T> execute(const sql &stmt, typename std::enable_if< !std::is_same<T, row>::value >::type* = 0)
  {
    std::cout << impl_->dialect()->direct(stmt) << "\n";
    return result<T>(impl_->execute(stmt));
  }

  template < class T >
  statement<T> prepare(const matador::sql &sql, typename std::enable_if< !std::is_same<T, row>::value >::type* = 0)
  {
    std::cout << "PSQL: " << impl_->dialect()->direct(sql) << "\n";
    return statement<T>(impl_->prepare(sql));
  }

  template < class T >
  statement<T> prepare(const matador::sql &sql, const std::string &tablename, row prototype, typename std::enable_if< std::is_same<T, row>::value >::type* = 0)
  {
    prepare_prototype_row(prototype, tablename);
    std::cout << "PSQL: " << impl_->dialect()->direct(sql) << "\n";
    return statement<T>(impl_->prepare(sql), prototype);
  }

private:
  connection_impl* create_connection(const std::string &type) const;
  void init_from_foreign_connection(const connection &foreign_connection);
  void parse_dns(const std::string &dns);

private:
  std::string type_;
  std::string dns_;
  std::unique_ptr<connection_impl> impl_;
};

}
#endif /* CONNECTION_HPP */
