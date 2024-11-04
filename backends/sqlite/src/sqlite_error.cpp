#include "sqlite_error.hpp"

#include <stdexcept>

#include <sqlite3.h>

namespace matador::backends::sqlite {

bool is_not_ok_or_done(const int ec)
{
  return ec != SQLITE_OK && ec != SQLITE_DONE;
}

sql::sql_error make_error(const sql::sql_error_code ec, const int err, sqlite3 *db, const std::string &sql)
{
  return sql::sql_error{ec, std::to_string(err), sqlite3_errmsg(db), "sqlite", sql};
}

}