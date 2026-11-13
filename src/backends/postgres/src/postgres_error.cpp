#include "postgres_error.hpp"

#include <sstream>

#include <libpq-fe.h>

namespace matador::backends::postgres {

error make_error(const query::error_code ec,
                        const PGresult *res,
                        const PGconn *db,
                        const std::string &msg,
                        const std::string &sql) {
  error err(ec, msg);
  err.add_error_info("dbms", "postgres");
  if (!sql.empty()) {
    err.add_error_info("sql", sql);
  }
  if (res == nullptr) {
    err.add_error_info("message", PQerrorMessage(db));
  } else if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
    err.add_error_info("message", PQresultErrorField(res, PG_DIAG_SQLSTATE));
  }

  return err;
}

bool is_result_error(const PGresult *res) {
  if (res == nullptr) {
    return true;
  }
  const auto status = PQresultStatus(res);
  return status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK;
}

static void throw_postgres_error(const char *what, const std::string &source) {
  std::stringstream msg;
  msg << "postgres error (" << source << "): " << what;
  throw std::logic_error(msg.str());
}

static void throw_postgres_error(const PGconn *db, const std::string &source) {
  if (PQstatus(db) == CONNECTION_BAD) {
    throw_postgres_error(PQerrorMessage(db), source);
  }
}

static void throw_postgres_error(const PGresult *res, const PGconn *db, const std::string &source, const std::string &sql) {
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