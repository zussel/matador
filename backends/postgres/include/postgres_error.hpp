#ifndef QUERY_POSTGRES_ERROR_HPP
#define QUERY_POSTGRES_ERROR_HPP

#include <libpq-fe.h>

#include <string>

namespace matador::backends::postgres {

void throw_postgres_error(const char *what, const std::string &source);
void throw_postgres_error(PGconn *db, const std::string &source);
void throw_postgres_error(PGresult *res, PGconn *db, const std::string &source, const std::string &sql);

}

#endif //QUERY_POSTGRES_ERROR_HPP
