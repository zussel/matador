//
// Created by sascha on 24.05.19.
//

#ifndef MATADOR_POSTGRESQL_EXCEPTION_HPP
#define MATADOR_POSTGRESQL_EXCEPTION_HPP

#include <string>
#include <libpq-fe.h>

#include "matador/sql/sql_exception.hpp"

namespace matador {

namespace postgresql {

void throw_database_error(PGresult *res, PGconn *db, const std::string &source, const std::string &sql = "");

}

}

#endif //MATADOR_POSTGRESQL_EXCEPTION_HPP
