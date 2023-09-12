#include "matador/db/postgresql/postgresql_connection.hpp"
#include "matador/db/postgresql/postgresql_statement.hpp"
#include "matador/db/postgresql/postgresql_exception.hpp"
#include "matador/db/postgresql/postgresql_result.hpp"

#include "matador/sql/connection_info.hpp"
#include "matador/sql/database_error.hpp"

#include <libpq-fe.h>

#include <regex>

namespace matador {
namespace postgresql {

postgresql_connection::postgresql_connection()
  :is_open_(false)
{}

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

void postgresql_connection::open(const connection_info &info)
{
  if (is_open_) {
    return;
  }

  std::string connection("user=" + info.user + " password=" + info.password + " host=" + info.hostname + " dbname=" + info.database + " port=" + std::to_string(info.port));

  conn_ = PQconnectdb(connection.c_str());
  if (PQstatus(conn_) == CONNECTION_BAD) {
    std::string msg = PQerrorMessage(conn_);
    PQfinish(conn_);
    throw database_error(msg, "postgresql", "42000");
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
  return execute_internal(stmt);
}

detail::result_impl *postgresql_connection::execute(const std::string &stmt)
{
  return execute_internal(stmt);
}

detail::statement_impl *postgresql_connection::prepare(const matador::sql &stmt)
{
  return new postgresql_statement(*this, stmt);
}

void postgresql_connection::begin()
{
  // TODO: check result
  std::unique_ptr<postgresql_result> res(execute_internal("START TRANSACTION;"));
}

void postgresql_connection::commit()
{
  // TODO: check result
  std::unique_ptr<postgresql_result> res(execute_internal("COMMIT;"));
}

void postgresql_connection::rollback()
{
  // TODO: check result
  std::unique_ptr<postgresql_result> res(execute_internal("ROLLBACK;"));
}

std::string postgresql_connection::type() const
{
  return "postgresql";
}

std::string postgresql_connection::client_version() const
{
  return "";
}

std::string postgresql_connection::server_version() const
{
  return "";
}

bool postgresql_connection::exists(const std::string &tablename)
{
  std::string stmt("SELECT 1 FROM information_schema.tables WHERE table_schema = 'public' AND table_name = '" + tablename + "'");

  std::unique_ptr<postgresql_result> res(execute_internal(stmt));
  return res->result_rows() == 1;
}

std::vector<field> postgresql_connection::describe(const std::string &table)
{
  std::string stmt(
    "SELECT ordinal_position, column_name, udt_name, data_type, is_nullable, column_default FROM information_schema.columns WHERE table_schema='public' AND table_name='" + table + "'");

  std::unique_ptr<postgresql_result> res(execute_internal(stmt));

  std::vector<field> fields;

  if (res->result_rows() == 0) {
    return fields;
  }

  do {
    field f;

    char *end = nullptr;
    f.index(strtoul(res->column(0), &end, 10) - 1);
    f.name(res->column(1));
    f.type(dialect_.string_type(res->column(2)));
    end = nullptr;
    f.not_null(strtoul(res->column(4), &end, 10) == 0);
    f.default_value(res->column(5));
    fields.push_back(f);
  } while(res->fetch());

  return fields;
}

PGconn *postgresql_connection::handle() const
{
  return conn_;
}

basic_dialect *postgresql_connection::dialect()
{
  return &dialect_;
}

postgresql_result* postgresql_connection::execute_internal(const std::string &stmt)
{
  PGresult *res = PQexec(conn_, stmt.c_str());

  throw_database_error(res, conn_, "postgresql", stmt);

  return new postgresql_result(res);
}

unsigned short postgresql_connection::default_port() const
{
  return 5432;
}

}
}

extern "C"
{
MATADOR_POSTGRESQL_API matador::connection_impl* create_database()
{
  return new matador::postgresql::postgresql_connection();
}

MATADOR_POSTGRESQL_API void destroy_database(matador::connection_impl *db)
{
  delete db;
}
}
