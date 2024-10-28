#ifndef SQL_ERROR_HPP
#define SQL_ERROR_HPP

#include <cstdint>
#include <string>

namespace matador::sql {

enum class sql_error_code : uint8_t {
    OK = 0,
    INVALID_QUERY,
    UNKNOWN_TABLE,
    UNKNOWN_COLUMN,
    BIND_FAILED,
    EXECUTE_FAILED,
    FAILURE
};

class sql_error
{
public:
  sql_error(sql_error_code ec,
            std::string native_error,
            std::string error_message,
            std::string dbms,
            std::string sql_statement = "");

  [[nodiscard]] sql_error_code ec() const;
  [[nodiscard]] std::string native_error() const;
  [[nodiscard]] std::string error_message() const;
  [[nodiscard]] std::string dbms() const;
  [[nodiscard]] std::string sql_statement() const;

private:
//  std::error_code error_code_;
  sql_error_code error_code_;
  std::string native_error_;
  std::string error_message_;
  std::string dbms_;
  std::string sql_statement_;
};
}

#endif //SQL_ERROR_HPP
