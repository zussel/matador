#ifndef QUERY_ODBC_ERROR_HPP
#define QUERY_ODBC_ERROR_HPP

#include <string>

struct sqlite3;

namespace matador::backends::odbc {

void throw_odbc_error(int ec, sqlite3 *db, const std::string &source);
void throw_odbc_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql);

}

#endif //QUERY_ODBC_ERROR_HPP
