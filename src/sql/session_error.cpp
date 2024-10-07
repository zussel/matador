#include "matador/sql/session_error.hpp"

namespace matador::sql {

session_error::session_error(session_error_code ec,
                             std::string error_message,
                             std::optional<sql::sql_error> sql_err)
: error_code_(ec)
, error_message_(std::move(error_message))
, sql_error_(std::move(sql_err))
{}

session_error_code session_error::ec() const
{
  return error_code_;
}

std::string session_error::error_message() const
{
  return error_message_;
}

std::optional<sql_error> session_error::sql_error() const
{
  return sql_error_;
}

}