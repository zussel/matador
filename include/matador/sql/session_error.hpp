#ifndef MATADOR_SESSION_ERROR_HPP
#define MATADOR_SESSION_ERROR_HPP

#include "matador/sql/sql_error.hpp"

#include <optional>
#include <string>

namespace matador::sql {


enum class session_error_code {
  Ok = 0,
  NoConnectionAvailable,
  UnknownType,
  FailedToBuildQuery,
  FailedToFindObject
};

class session_error
{
public:
  session_error(session_error_code ec,
                std::string error_message,
                std::optional<sql::sql_error> sql_err = std::nullopt);

  [[nodiscard]] session_error_code ec() const;
  [[nodiscard]] std::string error_message() const;
  [[nodiscard]] std::optional<sql::sql_error> sql_error() const;

private:
  session_error_code error_code_;
  std::string error_message_;
  std::optional<sql::sql_error> sql_error_;
};

}

#endif //MATADOR_SESSION_ERROR_HPP
