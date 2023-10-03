#include "matador/db/sqlite/sqlite_connection.hpp"
#include "matador/db/sqlite/sqlite_statement.hpp"
#include "matador/db/sqlite/sqlite_result.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"

#include "matador/sql/sql.hpp"
#include "matador/sql/connection_info.hpp"

#include <sqlite3.h>

using namespace std::placeholders;

namespace matador {
  
namespace sqlite {
  
sqlite_connection::sqlite_connection()
  : sqlite_db_(nullptr)
{
}

sqlite_connection::~sqlite_connection()
{
  close();
}


void sqlite_connection::open(const connection_info &info)
{
  int ret = sqlite3_open(info.database.c_str(), &sqlite_db_);

  throw_database_error(ret, sqlite_db_, "sqlite");
}

bool sqlite_connection::is_open() const
{
  return sqlite_db_ != nullptr;
}

void sqlite_connection::close()
{
  int ret = sqlite3_close(sqlite_db_);

  throw_database_error(ret, sqlite_db_, "sqlite_close");

  sqlite_db_ = nullptr;
}

sqlite3* sqlite_connection::handle()
{
  return sqlite_db_;
}

void sqlite_connection::begin()
{
  std::unique_ptr<sqlite_result> res(execute_internal("BEGIN TRANSACTION;"));
}

void sqlite_connection::commit()
{
  std::unique_ptr<sqlite_result> res(execute_internal("COMMIT TRANSACTION;"));
}

void sqlite_connection::rollback()
{
  std::unique_ptr<sqlite_result> res(execute_internal("ROLLBACK TRANSACTION;"));
}

std::string sqlite_connection::type() const
{
  return "sqlite";
}

version sqlite_connection::client_version() const
{
  return { static_cast<unsigned int>(SQLITE_VERSION_NUMBER / 1000000),
           static_cast<unsigned int>((SQLITE_VERSION_NUMBER % 1000000) / 1000),
           static_cast<unsigned int>(SQLITE_VERSION_NUMBER % 1000) };
}

version sqlite_connection::server_version() const
{
  return { static_cast<unsigned int>(SQLITE_VERSION_NUMBER / 100000),
           static_cast<unsigned int>((SQLITE_VERSION_NUMBER % 100000) / 1000),
           static_cast<unsigned int>(SQLITE_VERSION_NUMBER % 1000) };
}

matador::detail::result_impl* sqlite_connection::execute(const matador::sql &sql)
{
  std::string stmt = dialect_.direct(sql);
  return execute_internal(stmt);
}

matador::detail::result_impl* sqlite_connection::execute(const std::string &stmt)
{
  return execute_internal(stmt);
}

matador::detail::statement_impl *sqlite_connection::prepare(const matador::sql &sql)
{
  return new sqlite_statement(*this, sql);
}

bool sqlite_connection::exists(const std::string &table_name)
{
  std::string stmt("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND tbl_name='" + table_name + "' LIMIT 1");
  std::unique_ptr<sqlite_result> res((execute_internal(stmt)));

  if (res->result_rows() != 1) {
    return false;
  } else {
    char *end;
    return strtoul(res->column(0), &end, 10) == 1;
  }
}

std::vector<field> sqlite_connection::describe(const std::string &table)
{
  std::string stmt("PRAGMA table_info(" + table + ")");
  std::unique_ptr<sqlite_result> res((execute_internal(stmt)));

  std::vector<field> fields;

  do {
    field f;
    char *end = nullptr;
    f.index(strtoul(res->column(0), &end, 10));
    f.name(res->column(1));
    f.type(dialect_.string_type(res->column(2)));
    end = nullptr;
    f.not_null(strtoul(res->column(3), &end, 10) == 0);
    f.default_value(res->column(4));
//    end = nullptr;
//    f.is_primary_key(strtoul(res->column(3), &end, 10) == 0);
    fields.push_back(f);
  } while (res->fetch());

  return fields;
}

basic_dialect *sqlite_connection::dialect()
{
  return &dialect_;
}

int sqlite_connection::parse_result(void* param, int column_count, char** values, char** /*columns*/)
{
  auto *result = static_cast<sqlite_result*>(param);

  /********************
   *
   * a new row was retrieved
   * add a new row to the result
   * and iterator over all columns
   * adding each column value as
   * string to the row
   *
   ********************/

  result->push_back(values, column_count);
  return 0;
}

unsigned long sqlite_connection::last_inserted_id()
{
  return static_cast<unsigned long>(sqlite3_last_insert_rowid(sqlite_db_));
}

sqlite_result *sqlite_connection::execute_internal(const std::string &stmt)
{
  std::unique_ptr<sqlite_result> res(new sqlite_result);
  char *errmsg = nullptr;
  int ret = sqlite3_exec(sqlite_db_, stmt.c_str(), parse_result, res.get(), &errmsg);

  throw_database_error(ret, sqlite_db_, "sqlite", stmt);

  return res.release();
}

unsigned short sqlite_connection::default_port() const
{
  return 0;
}

}

}

extern "C"
{
  OOS_SQLITE_API matador::connection_impl* create_database()
  {
    return new matador::sqlite::sqlite_connection();
  }

  OOS_SQLITE_API void destroy_database(matador::connection_impl *db)
  {
    delete db;
  }
}
