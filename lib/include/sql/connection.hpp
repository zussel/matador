#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/result.hpp"
#include "sql/statement.hpp"
#include "sql/connection_impl.hpp"
#include "row.hpp"
#include "field.hpp"

#include <string>

namespace oos {

class basic_dialect;

/**
 * @brief The connection class represents a connection to a database.
 */
class OOS_API connection
{
public:
  /**
   * @brief Creates a database connection from a connection string.
   *
   * @param dns The database connection string
   */
  explicit connection(const std::string &dns);
  ~connection();

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

private:
  template < class T >
  friend class query;

  void prepare_prototype_row(row &prototype, const std::string &tablename);

  template < class T >
  result<T> execute(const sql &stmt, const std::string &tablename, row prototype, typename std::enable_if< std::is_same<T, row>::value >::type* = 0)
  {
//    prototype.clear();
    // get column descriptions
    prepare_prototype_row(prototype, tablename);
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
    return result<T>(impl_->execute(stmt));
  }

  template < class T >
  statement<T> prepare(const oos::sql &sql, typename std::enable_if< !std::is_same<T, row>::value >::type* = 0)
  {
    return statement<T>(impl_->prepare(sql));
  }

  template < class T >
  statement<T> prepare(const oos::sql &sql, const std::string &tablename, row prototype, typename std::enable_if< std::is_same<T, row>::value >::type* = 0)
  {
    prepare_prototype_row(prototype, tablename);
    return statement<T>(impl_->prepare(sql));
  }

private:
  std::string type_;
  std::string dns_;
  connection_impl *impl_;
};

}
#endif /* CONNECTION_HPP */
