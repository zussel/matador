#include "matador/sql/database_error.hpp"

#include "matador/db/postgresql/postgresql_exception.hpp"

namespace matador {

namespace postgresql {

void throw_database_error(PGresult *res, PGconn *db, const std::string &source, const std::string &sql)
{
  if (res == nullptr ||
      (PQresultStatus(res) != PGRES_COMMAND_OK &&
       PQresultStatus(res) != PGRES_TUPLES_OK)) {
    throw database_error(PQerrorMessage(db), source, PQresultErrorField(res, PG_DIAG_SQLSTATE), sql);
  }
}

}
}