#ifndef QUERY_SQLITE_ERROR_HPP
#define QUERY_SQLITE_ERROR_HPP

#include "matador/sql/sql_error.hpp"

#include <string>

struct sqlite3;

namespace matador::backends::sqlite {

bool is_not_ok_or_done(int ec);

sql::sql_error make_error(sql::sql_error_code ec, int err, sqlite3 *db, const std::string &sql = "");

}

#endif //QUERY_SQLITE_ERROR_HPP
