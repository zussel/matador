//
// Created by sascha on 24.05.19.
//

#include <libpq-fe.h>
#include <regex>

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

void postgresql_connection::open(const std::string &dns)
{
  if (is_open_) {
    return;
  }
  static const std::regex DNS_RGX("(.+?)(?::(.+?))?@([^:]+?)(?::([1-9][0-9]*?))?/(.+)");
  std::smatch what;

  if (!std::regex_match(dns, what, DNS_RGX)) {
    throw_error("mysql:open", "invalid dns: " + dns);
  }

  const std::string user = what[1].str();
  const std::string passwd = what[2].str();
  const std::string host = what[3].str();

  std::string port = "5432";
  if (what[4].matched) {
    port = what[4].str();
  }

  db_ = what[5].str();


  std::string connection("user=" + user + " password=" + passwd + " host=" + host + " dbname=" + db_ + " port=" + port);

  conn_ = PQconnectdb(connection.c_str());
  if (PQstatus(conn_) == CONNECTION_BAD) {
    PQfinish(conn_);
    throw postgresql_exception(conn_, "mysql_query", dns);
  }

  is_open_ = true;
}

void postgresql_connection::close()
{
  if (is_open_) {
    PQfinish(conn_);
    is_open_ = false;
  }
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
  return "postgresql";
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


extern "C"
{
OOS_MYSQL_API matador::connection_impl* create_database()
{
  return new matador::postgresql::postgresql_connection();
}

OOS_MYSQL_API void destroy_database(matador::connection_impl *db)
{
  delete db;
}
}
