#ifndef QUERY_POSTGRES_ERROR_HPP
#define QUERY_POSTGRES_ERROR_HPP

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <string>

namespace matador::backends::mysql {

void throw_mysql_error(const char *what, const std::string &source);
void throw_mysql_error(MYSQL *db, const std::string &source);
void throw_mysql_error(MYSQL_STMT *stmt, const std::string &source, const std::string &sql);

}

#endif //QUERY_POSTGRES_ERROR_HPP
