//
// Created by sascha on 24.05.19.
//

#include <libpq-fe.h>

#include "matador/db/postgresql/postgresql_connection.hpp"
#include "matador/db/postgresql/postgresql_exception.hpp"

namespace matador {
namespace postgresql {

postgresql_connection::postgresql_connection()
  :is_open_(false)
{

}

postgresql_connection::~postgresql_connection()
{
  close();
}

bool postgresql_connection::is_open() const
{
  return is_open_;
}

unsigned long postgresql_connection::last_inserted_id()
{
  return 0;
}

void postgresql_connection::open(const std::string &db)
{

  PGconn *conn = PQconnectdb(db.c_str());
  if (PQstatus(conn) == CONNECTION_BAD) {
    throw postgresql_exception(conn, "mysql_query", sql);
  }
}

void postgresql_connection::close()
{

}

detail::result_impl *postgresql_connection::execute(const matador::sql &sql)
{
  std::string stmt = dialect_.direct(sql);
  return execute(stmt);
}

detail::result_impl *postgresql_connection::execute(const std::string &stmt)
{
  return nullptr;
}

detail::statement_impl *postgresql_connection::prepare(const matador::sql &stmt)
{
  return nullptr;
}

void postgresql_connection::begin()
{

}

void postgresql_connection::commit()
{

}

void postgresql_connection::rollback()
{

}

std::string postgresql_connection::type() const
{
  return std::__cxx11::string();
}

std::string postgresql_connection::version() const
{
  return std::__cxx11::string();
}

bool postgresql_connection::exists(const std::string &tablename)
{
  return false;
}

std::vector<field> postgresql_connection::describe(const std::string &table)
{
  return std::vector<field>();
}

basic_dialect *postgresql_connection::dialect()
{
  return nullptr;
}
}
}