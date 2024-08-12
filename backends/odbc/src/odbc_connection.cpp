#include "odbc_connection.hpp"
#include "odbc_error.hpp"
#include "odbc_result_reader.hpp"
#include "odbc_statement.hpp"

#include "matador/sql/record.hpp"

#include "matador/utils/data_types.hpp"

#include <cstring>
#include <memory>
#include <utility>

namespace matador::backends::odbc {

odbc_connection::odbc_connection(const sql::connection_info &info)
: connection_impl(info) {
}

void odbc_connection::open()
{
  if (is_open()) {
    return;
  }

  const auto ret = sqlite3_open(info().database.c_str(), &db_);

  if (ret != odbc_OK) {
    throw_odbc_error(ret, db_, "open");
  }
}

void odbc_connection::close()
{
  int ret = sqlite3_close(db_);

  throw_odbc_error(ret, db_, "close");

  db_ = nullptr;
}

bool odbc_connection::is_open() const
{
  return db_ != nullptr;
}

bool odbc_connection::is_valid() const
{
  SQLUINTEGER connectionDead;
  SQLRETURN ret = SQLGetConnectAttr(connectionHandle_, SQL_ATTR_CONNECTION_DEAD, &connectionDead, 0, NULL);
  return (ret == SQL_SUCCESS && connectionDead == SQL_CD_FALSE);
}

int odbc_connection::parse_result(void* param, int column_count, char** values, char** columns)
{
  auto *context = static_cast<fetch_context*>(param);

  odbc_result_reader::columns column;
  for(int i = 0; i < column_count; ++i) {
    // copy and store column data;
    if (values[i] == nullptr) {
      auto val = new char[1];
      val[0] = '\0';
      column.push_back(val);
    } else {
      size_t size = strlen(values[i]);
      auto val = new char[size + 1];
      std::memcpy(val, values[i], size);
      val[size] = '\0';
      column.push_back(val);
    }
  }
  context->rows.emplace_back(column);

  if (context->prototype.empty()) {
    for(int i = 0; i < column_count; ++i) {
      context->prototype.emplace_back(columns[i]);
    }
  }

  return 0;
}

odbc_connection::fetch_context odbc_connection::fetch_internal(const std::string &stmt)
{
  fetch_context context;
  char *errmsg = nullptr;
  const int ret = sqlite3_exec(db_, stmt.c_str(), parse_result, &context, &errmsg);

  throw_odbc_error(ret, db_, "sqlite", stmt);

  return context;
}

size_t odbc_connection::execute(const std::string &stmt)
{
  char *errmsg = nullptr;
  int ret = sqlite3_exec(db_, stmt.c_str(), nullptr, nullptr, &errmsg);

  throw_odbc_error(ret, db_, "sqlite", stmt);

  return sqlite3_changes(db_);
}

std::unique_ptr<sql::query_result_impl> odbc_connection::fetch(const std::string &stmt)
{
  auto context = fetch_internal(stmt);

  return std::move(std::make_unique<sql::query_result_impl>(std::make_unique<odbc_result_reader>(std::move(context.rows), context.prototype.size()), std::move(context.prototype)));
}

std::unique_ptr<sql::statement_impl> odbc_connection::prepare(sql::query_context query)
{
  sqlite3_stmt *stmt{};
  int ret = sqlite3_prepare_v2(db_, query.sql.c_str(), static_cast<int>(query.sql.size()), &stmt, nullptr);
  throw_odbc_error(ret, db_, "sqlite3_prepare_v2", query.sql);

  return std::make_unique<odbc_statement>(db_, stmt, query);
}

data_type string2type(const char *type)
{
  if (strncmp(type, "INTEGER", 7) == 0) {
    return data_type::type_int;
  } else if (strncmp(type, "TINYINT", 7) == 0) {
    return data_type::type_char;
  } else if (strncmp(type, "SMALLINT", 8) == 0) {
    return data_type::type_short;
  } else if (strncmp(type, "BIGINT", 6) == 0) {
    return data_type::type_long_long;
  } else if (strcmp(type, "BOOLEAN") == 0) {
    return data_type::type_bool;
  } else if (strcmp(type, "REAL") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "FLOAT") == 0) {
    return data_type::type_float;
  } else if (strcmp(type, "DOUBLE") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "BLOB") == 0) {
    return data_type::type_blob;
  } else if (strcmp(type, "NULL") == 0) {
    return data_type::type_null;
  } else if (strncmp(type, "VARCHAR", 7) == 0) {
    return data_type::type_varchar;
  } else if (strcmp(type, "DATE") == 0) {
    return data_type::type_date;
  } else if (strcmp(type, "DATETIME") == 0) {
    return data_type::type_time;
  } else if (strcmp(type, "TEXT") == 0) {
    return data_type::type_text;
  } else {
    return data_type::type_unknown;
  }
}

std::vector<sql::column_definition> odbc_connection::describe(const std::string& table)
{
  const auto result = fetch_internal("PRAGMA table_info(" + table + ")");

  odbc_result_reader reader(result.rows, result.prototype.size());
  std::vector<sql::column_definition> prototype;
  while (reader.fetch()) {
    char *end = nullptr;
    // Todo: add index to column
    auto index = strtoul(reader.column(0), &end, 10);
    std::string name = reader.column(1);

    auto type = (string2type(reader.column(2)));
    end = nullptr;
    sql::null_option null_opt{sql::null_option::NULLABLE};
    if (strtoul(reader.column(3), &end, 10) == 0) {
      null_opt = sql::null_option::NOT_NULL;
    }
    // f.default_value(res->column(4));
    prototype.emplace_back(name, type, utils::null_attributes, null_opt, index);
  }

  return std::move(prototype);
}

bool odbc_connection::exists(const std::string &schema_name, const std::string &table_name)
{
  const auto result = fetch_internal("SELECT COUNT(*) FROM odbc_master WHERE type='table' AND tbl_name='" + table_name + "' LIMIT 1");
  odbc_result_reader reader(result.rows, result.prototype.size());

  if (!reader.fetch()) {
    // Todo: throw an exception?
    return false;
  }

  int v{};
  reader.read_value(nullptr, 0, v);

  return v == 1;
}

}

extern "C"
{
MATADOR_ODBC_API matador::sql::connection_impl *create_database(const matador::sql::connection_info &info)
{
  return new matador::backends::sqlite::odbc_connection(info);
}

MATADOR_ODBC_API void destroy_database(matador::sql::connection_impl *db)
{
  delete db;
}

}
