#include "matador/sql/database_error.hpp"

#include <utility>

namespace matador {

database_error::database_error(const char *what, std::string source, long ec, std::string sql)
  : std::runtime_error(what)
  , source_(std::move(source))
  , error_code_(ec)
  , sql_stmt_(std::move(sql))
{}

database_error::database_error(const char *what, std::string source, std::string sqlstate, long ec,
                               std::string sql)
  : std::runtime_error(what)
  , source_(std::move(source))
  , error_code_(ec)
  , sqlstate_(std::move(sqlstate))
  , sql_stmt_(std::move(sql))
{}

database_error::database_error(const char *what, std::string source, std::string sqlstate, std::string sql)
  : std::runtime_error(what)
  , source_(std::move(source))
  , sqlstate_(std::move(sqlstate))
  , sql_stmt_(std::move(sql))
{}

const char *database_error::source() const noexcept
{
  return source_.c_str();
}

long database_error::error_code() const noexcept
{
  return error_code_;
}

const char *database_error::sql_state() const noexcept
{
  return sqlstate_.c_str();
}

const char *database_error::sql_statement() const noexcept
{
  return sql_stmt_.c_str();
}
}