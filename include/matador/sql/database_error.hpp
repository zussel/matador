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

class OOS_SQL_API database_error : public std::runtime_error
{
public:
  database_error(const std::string &what, std::string source, long ec, std::string sql = "");
  database_error(const std::string &what, std::string source, std::string sqlstate, long ec, std::string sql = "");
  database_error(const std::string &what, std::string source, std::string sqlstate, std::string sql = "");

  ~database_error() noexcept override = default;

  const char* source() const noexcept;
  long error_code() const noexcept;
  const char* sql_state() const noexcept;
  const char* sql_statement() const noexcept;

private:
  std::string source_;
  long error_code_ = 0;
  std::string sqlstate_;
  std::string sql_stmt_;
};

}
#endif //MATADOR_DATABASE_ERROR_HPP
