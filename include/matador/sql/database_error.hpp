#ifndef MATADOR_DATABASE_ERROR_HPP
#define MATADOR_DATABASE_ERROR_HPP

#include <stdexcept>

namespace matador {

/**
 * @brief Thrown by db backend containing all available error information's
 *
 * This exception is thrown by the database backend and contains
 * all available error information's provided by the backend
 */
class database_error : public std::runtime_error
{
public:
  /**
   * Creates a database error exception with error code.
   *
   * @param what Error message
   * @param source Database vendor string
   * @param ec Error code
   * @param sql Optional sql statement string
   */
  database_error(const char *what, std::string source, long ec, std::string sql = "");

  /**
   * Creates a database error exception with sql state and error code.
   *
   * @param what Error message
   * @param source Database vendor string
   * @param sqlstate SQL State
   * @param ec Error code
   * @param sql Optional sql statement string
   */
  database_error(const char *what, std::string source, std::string sqlstate, long ec, std::string sql = "");

  /**
   * Creates a database error exception with sql state.
   *
   * @param what Error message
   * @param source Database vendor string
   * @param sqlstate SQL State
   * @param sql Optional sql statement string
   */
  database_error(const char *what, std::string source, std::string sqlstate, std::string sql = "");

  /**
   * Destroy the database error
   */
  ~database_error() noexcept override = default;

  /**
   * Returns the database vendor string.
   *
   * @return Database vendor string
   */
  const char* source() const noexcept;

  /**
   * Returns a proprietary error code depending
   * on the database backend.
   *
   * @return Proprietary database error code
   */
  long error_code() const noexcept;

  /**
   * Returns the sql state according to the SQL standard
   * provided by the database backend. It is a 5 char long
   * string representing a sql error code.
   *
   * <a href="https://en.wikipedia.org/wiki/SQLSTATE">SQL States</a>
   *
   * @return SQL State
   */
  const char* sql_state() const noexcept;

  /**
   * If available it returns a corresponding sql statement.
   *
   * @return Optional corresponding sql statement
   */
  const char* sql_statement() const noexcept;

private:
  std::string source_;
  long error_code_ = 0;
  std::string sqlstate_;
  std::string sql_stmt_;
};

}
#endif //MATADOR_DATABASE_ERROR_HPP
