//
// Created by sascha on 28.12.19.
//

#ifndef MATADOR_DATABASE_ERROR_HPP
#define MATADOR_DATABASE_ERROR_HPP

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

#include <stdexcept>

namespace matador {

/**
 * @brief Thrown by db backend containing all available error informations
 *
 * This exception is thrown by the database backend and contains
 * all available error informations provided by the backend
 */
class OOS_SQL_API database_error : public std::runtime_error
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
  database_error(const std::string &what, std::string source, long ec, std::string sql = "");

  /**
   * Creates a database error exception with sql state and error code.
   *
   * @param what Error message
   * @param source Database vendor string
   * @param sqlstate SQL State
   * @param ec Error code
   * @param sql Optional sql statement string
   */
  database_error(const std::string &what, std::string source, std::string sqlstate, long ec, std::string sql = "");

  /**
   * Creates a database error exception with sql state.
   *
   * @param what Error message
   * @param source Database vendor string
   * @param sqlstate SQL State
   * @param sql Optional sql statement string
   */
  database_error(const std::string &what, std::string source, std::string sqlstate, std::string sql = "");

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
