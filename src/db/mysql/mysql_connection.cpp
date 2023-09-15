#include "matador/db/mysql/mysql_connection.hpp"
#include "matador/db/mysql/mysql_statement.hpp"
#include "matador/db/mysql/mysql_result.hpp"
#include "matador/db/mysql/mysql_types.hpp"
#include "matador/db/mysql/mysql_exception.hpp"

#include "matador/sql/connection_info.hpp"

#include <sstream>
#include <regex>

using namespace std::placeholders;

namespace matador {
  
namespace mysql {
  
mysql_connection::mysql_connection()
  : is_open_(false)
{
}

mysql_connection::~mysql_connection()
{
  close();
}


void mysql_connection::open(const connection_info &info)
{
  if (!mysql_init(&mysql_)) {
    throw_error(&mysql_, "mysql");
  }

  if (!mysql_real_connect(&mysql_, info.hostname.c_str(), info.user.c_str(), !info.password.empty() ? info.password.c_str() : nullptr, info.database.c_str(), info.port, nullptr, 0)) {
    // disconnect all handles
    mysql_close(&mysql_);
    // throw exception
    throw_error(&mysql_, "mysql");
  }
  is_open_ = true;
}

bool mysql_connection::is_open() const
{
  return is_open_;
}

void mysql_connection::close()
{
  if (is_open_) {
    mysql_close(&mysql_);
  }

  // tell mysql to disconnect the library
  mysql_library_end();

  is_open_ = false;
}

MYSQL*mysql_connection::handle()
{
  return &mysql_;
}

detail::result_impl* mysql_connection::execute(const matador::sql &sql)
{
  std::string stmt = dialect_.direct(sql);
  return execute(stmt);
}

detail::result_impl* mysql_connection::execute(const std::string &sql)
{
  return execute_internal(sql);
}

detail::statement_impl* mysql_connection::prepare(const matador::sql &stmt)
{
  return new mysql_statement(*this, stmt);
}

void mysql_connection::begin()
{
  // TODO: check result
  std::unique_ptr<mysql_result> res(execute_internal("START TRANSACTION;"));
}

void mysql_connection::commit()
{
  // TODO: check result
  std::unique_ptr<mysql_result> res(execute_internal("COMMIT;"));
}

void mysql_connection::rollback()
{
  // TODO: check result
  std::unique_ptr<mysql_result> res(execute_internal("ROLLBACK;"));
}

std::string mysql_connection::type() const
{
  return "mysql";
}

std::string mysql_connection::client_version() const
{
  return mysql_get_server_info(const_cast<MYSQL*>(&mysql_));
}

std::string mysql_connection::server_version() const
{
  return mysql_get_client_info();
}

bool mysql_connection::exists(const std::string &tablename)
{
  std::string stmt("SHOW TABLES LIKE '" + tablename + "'");
  std::unique_ptr<mysql_result> res(execute_internal(stmt));
  return res->fetch();
}

std::vector<field> mysql_connection::describe(const std::string &table)
{
  std::string stmt("SHOW COLUMNS FROM " + table);
  std::unique_ptr<mysql_result> res(execute_internal(stmt));

  std::vector<field> fields;

  while (res->fetch()) {
    field f;
    f.name(res->column(0));
    f.type(dialect_.string_type(res->column(1)));
    f.index(fields.size());
    char *end;
    f.not_null(strtoul(res->column(2), &end, 10) == 0);

    fields.push_back(f);
  }

  return fields;
}

mysql_result *mysql_connection::execute_internal(const std::string &sql)
{
  if (mysql_query(&mysql_, sql.c_str())) {
    throw_error(&mysql_, "mysql_query", sql);
  }
  return new mysql_result(&mysql_);
}

basic_dialect* mysql_connection::dialect()
{
  return &dialect_;
}

unsigned long mysql_connection::last_inserted_id()
{
  if (mysql_field_count(&mysql_) == 0 &&
      mysql_insert_id(&mysql_) != 0)
  {
    return (unsigned long)mysql_insert_id(&mysql_);
  }
  return 0;
}

unsigned short mysql_connection::default_port() const
{
  return 3306;
}

}

}

extern "C"
{
  OOS_MYSQL_API matador::connection_impl* create_database()
  {
    return new matador::mysql::mysql_connection();
  }

  OOS_MYSQL_API void destroy_database(matador::connection_impl *db)
  {
    delete db;
  }
}
