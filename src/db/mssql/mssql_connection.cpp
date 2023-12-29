#include "matador/db/mssql/mssql_connection.hpp"
#include "matador/db/mssql/mssql_statement.hpp"
#include "matador/db/mssql/mssql_result.hpp"
#include "matador/db/mssql/mssql_exception.hpp"

#include "matador/sql/connection_info.hpp"
#include "matador/sql/database_error.hpp"

#include <regex>

using namespace std::placeholders;

namespace matador {
  
namespace mssql {

mssql_connection::mssql_connection()
  : odbc_(nullptr)
  , connection_(nullptr)
  , is_open_(false)
{
}

mssql_connection::~mssql_connection()
{
  close();
}


void mssql_connection::open(const connection_info &info)
{
  database_name_ = info.database;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &odbc_);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_database_error(ret, SQL_HANDLE_ENV, odbc_, "mssql");
  }

  ret = SQLSetEnvAttr(odbc_, SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3, 0);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_database_error(ret, SQL_HANDLE_ENV, odbc_, "mssql");
  }

  ret = SQLAllocHandle(SQL_HANDLE_DBC, odbc_, &connection_);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql");
  }

  SQLSetConnectAttr(connection_, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);

  std::string dns("DRIVER={" + info.driver + "};SERVER=" + info.hostname + ";Protocol=TCPIP;Port=" + std::to_string(info.port) + ";DATABASE=" + database_name_ + ";UID=" + info.user + ";PWD=" + info.password + ";");

  SQLCHAR retconstring[1024];
  ret = SQLDriverConnect(connection_, nullptr, (SQLCHAR*)dns.c_str(), SQL_NTS, retconstring, 1024, nullptr, SQL_DRIVER_NOPROMPT);

  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql");

  is_open_ = true;
}

bool mssql_connection::is_open() const
{
  return is_open_;
}

void mssql_connection::close()
{
  if (!is_open_) {
    return;
  }

  SQLRETURN ret = SQLDisconnect(connection_);

  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql");

  ret = SQLFreeHandle(SQL_HANDLE_DBC, connection_);
  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql");

  connection_ = nullptr;

  ret = SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
  throw_database_error(ret, SQL_HANDLE_ENV, odbc_, "mssql");

  odbc_ = nullptr;

  is_open_ = false;
}

matador::detail::result_impl *mssql_connection::execute(const matador::sql &sql)
{
  std::string stmt(dialect_.direct(sql));
  return execute(stmt);
}

detail::result_impl* mssql_connection::execute(const std::string &sqlstr)
{
  if (!connection_) {
    throw std::logic_error("mssql no odbc connection established");
  }
  // create statement handle
  SQLHANDLE stmt;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &stmt);
  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql", sqlstr);

  // execute statement
  ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlstr.c_str(), SQL_NTS);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql", sqlstr);

  return new mssql_result(stmt);
}

matador::detail::statement_impl *mssql_connection::prepare(detail::statement_context &&context) const
{
  return new mssql_statement(connection_, std::move(context));
}

void mssql_connection::begin()
{
  execute_no_result("BEGIN TRANSACTION;");
}

void mssql_connection::commit()
{
  execute_no_result("COMMIT;");
}

void mssql_connection::rollback()
{
  execute_no_result("ROLLBACK;");
}

std::string mssql_connection::type() const
{
  return "mssql";
}

version mssql_connection::client_version() const
{
  return { static_cast<unsigned int>(((ODBCVER & 0xF000) >> 12) * 10 + ((ODBCVER & 0x0F00) >> 8)),
           static_cast<unsigned int>(((ODBCVER & 0xF0) >> 4) * 10 + (ODBCVER & 0x0F)),
           0 };
}

version mssql_connection::server_version() const
{
  SQLCHAR dbms_ver[256];
  auto ret = SQLGetInfo(connection_, SQL_DBMS_VER, (SQLPOINTER)dbms_ver, sizeof(dbms_ver), NULL);

  if (ret == SQL_INVALID_HANDLE) {
    throw database_error("not connected", "mssql", -2);
  }

  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql");

  return version::from_string(reinterpret_cast< char const* >(dbms_ver));
}

bool mssql_connection::exists(const std::string &table_name)
{
  std::string stmt("SELECT TOP 1 COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_CATALOG='" + database_name_ + "' AND TABLE_NAME='" + table_name + "'");
  std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute(stmt)));

  if (!res->fetch()) {
    return false;
  } else {
    return res->get<int>(1) == 1;
  }
}

static database_type type2data_type(SQLSMALLINT type, size_t size);

std::vector<field> mssql_connection::describe(const std::string &table)
{
  // create statement handle
  SQLHANDLE stmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &stmt);
  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql");

  //std::string stmt("EXEC SP_COLUMNS " + table);

  SQLCHAR buf[256];
#ifdef _MSC_VER
  strcpy_s((char*)buf, 256, table.c_str());
#else
  strcpy((char*)buf, table.c_str());
#endif
  ret = SQLColumns(stmt, nullptr, 0, nullptr, 0, buf, SQL_NTS, nullptr, 0);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
  //std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute(stmt)));

  // bind to columns we need (column name, data type of column and index)
  size_t pos(0);
  size_t size(0);

  SQLCHAR column[64];
  SQLSMALLINT data_type(0);
  SQLCHAR type[64];
  SQLINTEGER not_null(0);
  SQLLEN indicator[6];

  // column name
  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column, sizeof(column), &indicator[0]);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
  // data type
  ret = SQLBindCol(stmt, 5, SQL_C_SSHORT, &data_type, sizeof(data_type), &indicator[1]);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
  // type name
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type, sizeof(type), &indicator[2]);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
  // size
  ret = SQLBindCol(stmt, 7, SQL_C_SLONG, &size, sizeof(size), &indicator[3]);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
  // nullable
  ret = SQLBindCol(stmt, 11, SQL_C_SSHORT, &not_null, 0, &indicator[4]);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
  // index (1 based)
  ret = SQLBindCol(stmt, 17, SQL_C_SLONG, &pos, 0, &indicator[5]);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");

  std::vector<field> fields;

  /* Fetch the data */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    field f;
    f.index(pos - 1);
    f.size(size);
    f.name(std::string((char*)column));

    f.type(type2data_type(data_type, size));
//    std::cout << "mssql column '" << column << "' type: " << data_type << "\n";
    //f.type(dialect_.string_type((char*)type));
    f.not_null(not_null == 0);

    fields.push_back(f);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  return fields;
}

database_type type2data_type(SQLSMALLINT type, size_t size)
{
  switch (type) {
  case SQL_CHAR:
    return database_type::type_char;
  case SQL_SMALLINT:
    return database_type::type_smallint;
  case SQL_INTEGER:
    return database_type::type_int;
  case SQL_BIGINT:
  case SQL_NUMERIC:
    return database_type::type_bigint;
  case SQL_TYPE_DATE:
  case -9:
    return database_type::type_date;
  case SQL_TYPE_TIMESTAMP:
    return database_type::type_time;
  case SQL_VARCHAR:
    return database_type::type_varchar;
  case SQL_REAL:
    return database_type::type_float;
  case SQL_FLOAT:
    return database_type::type_double;
  case SQL_BIT:
    return database_type::type_bool;
  case SQL_LONGVARCHAR:
    return (size != 2147483647 ? database_type::type_varchar : database_type::type_text);
  case SQL_UNKNOWN_TYPE:
  default:
    return database_type::type_unknown;
  }
}

SQLHANDLE mssql_connection::handle()
{
  return connection_;
}

unsigned long mssql_connection::last_inserted_id()
{
  std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute("select scope_identity()")));
  return res->get<unsigned long>(1);
}

basic_dialect* mssql_connection::dialect()
{
  return &dialect_;
}

void mssql_connection::execute_no_result(const std::string &stmt)
{
  if (!connection_) {
    throw std::logic_error("mssql no odbc connection established");
  }
  // create statement handle
  SQLHANDLE handle;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &handle);
  throw_database_error(ret, SQL_HANDLE_DBC, connection_, "mssql", stmt);

  // execute statement
//  int retry = retries_;
  ret = SQLExecDirectA(handle, (SQLCHAR*)stmt.c_str(), SQL_NTS);

  /*
  do {
    ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlstr.c_str(), SQL_NTS);
  } while (retry-- && !(SQL_SUCCEEDED(ret) || SQL_NO_DATA));
  */

  throw_database_error(ret, SQL_HANDLE_STMT, handle, "mssql", stmt);
}

unsigned short mssql_connection::default_port() const
{
  return 1433;
}

}

}

extern "C"
{
  OOS_MSSQL_API matador::connection_impl* create_database()
  {
    return new matador::mssql::mssql_connection();
  }

  OOS_MSSQL_API void destroy_database(matador::connection_impl *db)
  {
    delete db;
  }
}
