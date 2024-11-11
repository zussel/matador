#ifndef ODBC_HANDLE_HPP
#define ODBC_HANDLE_HPP

#include "odbc_error.hpp"

#include "matador/utils/result.hpp"

#include "matador/sql/sql_error.hpp"

namespace matador::backends::odbc {

class odbc_handle final {
public:
  odbc_handle(SQLSMALLINT handle_type);
  ~odbc_handle();

  utils::result<void, sql::sql_error> allocate();
  [[nodiscard]] utils::result<void, sql::sql_error> free();

  [[nodiscard]] bool is_valid() const;

  SQLHANDLE& handle();
  [[nodiscard]] const SQLHANDLE& handle() const;

  [[nodiscard]] SQLSMALLINT handle_type() const;

  utils::result<void, sql::sql_error> set_attribute(SQLINTEGER attr, SQLPOINTER value, SQLINTEGER string_length);

private:
  SQLHANDLE handle_{};
  SQLSMALLINT handle_type_{};
};

}

#endif //ODBC_HANDLE_HPP
