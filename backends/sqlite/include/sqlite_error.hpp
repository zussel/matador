#ifndef QUERY_SQLITE_ERROR_HPP
#define QUERY_SQLITE_ERROR_HPP

#include <string>

struct sqlite3;

namespace matador::backends::sqlite {

void throw_sqlite_error(int ec, sqlite3 *db, const std::string &source);
void throw_sqlite_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql);

}

#endif //QUERY_SQLITE_ERROR_HPP
