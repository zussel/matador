#ifndef QUERY_POSTGRES_ERROR_HPP
#define QUERY_POSTGRES_ERROR_HPP

#include "matador/utils/error.hpp"

#include "matador/query/runtime/error_code.hpp"

#include <libpq-fe.h>

#include <string>

namespace matador::backends::postgres {

error make_error(query::error_code ec,
                 const PGresult *res,
                 const PGconn *db,
                 const std::string &msg,
                 const std::string &sql = {});

bool is_result_error(const PGresult *res);

// void throw_postgres_error(const char *what, const std::string &source);
// void throw_postgres_error(PGconn *db, const std::string &source);
// void throw_postgres_error(PGresult *res, PGconn *db, const std::string &source, const std::string &sql);

}

#endif //QUERY_POSTGRES_ERROR_HPP
