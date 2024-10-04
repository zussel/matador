#include "postgres_error.hpp"

#include <sstream>

#include <libpq-fe.h>

namespace matador::backends::postgres {

void throw_postgres_error(const char *what, const std::string &source)
{
  std::stringstream msg;
  msg << "postgres error (" << source << "): " << what;
  throw std::logic_error(msg.str());
}

void throw_postgres_error(PGconn *db, const std::string &source)
{
  if (PQstatus(db) == CONNECTION_BAD) {
    throw_postgres_error(PQerrorMessage(db), source);
  }
}

void throw_postgres_error(PGresult *res, PGconn *db, const std::string &source, const std::string &sql)
{
  if (res == nullptr) {
    std::stringstream msg;
    msg << "postgres error (" << source << ", " << PQerrorMessage(db) << ": " << sql;
    throw std::logic_error(msg.str());
  }
  if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK &&
                                               status != PGRES_TUPLES_OK) {
    std::stringstream msg;
    msg << "postgres error (" << source << ", " << PQresultErrorField(res, PG_DIAG_SQLSTATE) << ") " << PQerrorMessage(db) << ": " << sql;
    throw std::logic_error(msg.str());
  }
}

}