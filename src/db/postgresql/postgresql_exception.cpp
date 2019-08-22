//
// Created by sascha on 24.05.19.
//

#include <sstream>

#include "matador/db/postgresql/postgresql_exception.hpp"

namespace matador {

namespace postgresql {

std::string error_message(PGconn *db, const std::string &source, const std::string &sql)
{
  std::stringstream msg;
  msg << source << ": " << PQerrorMessage(db) << " (" << sql << ")";
  return msg.str();
}

void throw_error(const std::string &source, const std::string &msg)
{
  throw postgresql_exception(source, msg);
}

void throw_error(int ec, PGconn *db, const std::string &source, const std::string &sql)
{
  if (ec == 0) {
    return;
  }
  throw postgresql_exception(db, source, sql);
}

postgresql_exception::postgresql_exception(const std::string &source, const std::string &what)
  : sql_exception("postgresql", (source + ": " + what).c_str())
{}


postgresql_exception::postgresql_exception(PGconn *db, const std::string &source, const std::string &what)
  : sql_exception("postgresql", error_message(db, source, what).c_str())
{}

postgresql_stmt_exception::postgresql_stmt_exception(const std::string &what)
  : sql_exception("postgresql", what.c_str())
{}

}
}