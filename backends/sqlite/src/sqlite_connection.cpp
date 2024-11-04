#include "sqlite_connection.hpp"
#include "sqlite_error.hpp"
#include "sqlite_result_reader.hpp"
#include "sqlite_statement.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/string.hpp"

#include <cstring>
#include <memory>
#include <utility>

namespace matador::backends::sqlite {
sqlite_connection::sqlite_connection(const sql::connection_info &info)
  : connection_impl(info) {
}

void sqlite_connection::open() {
  if (is_open()) {
    return;
  }

  if (const auto ret = sqlite3_open(info().database.c_str(), &db_); ret != SQLITE_OK) {
    make_error(sql::sql_error_code::OPEN_ERROR, ret, db_);
  }
}

void sqlite_connection::close() {
  if (!is_open()) {
    return;
  }

  if (const auto ret = sqlite3_close(db_); ret != SQLITE_OK) {
    make_error(sql::sql_error_code::CLOSE_ERROR, ret, db_);
  }

  db_ = nullptr;
}

bool sqlite_connection::is_open() const {
  return db_ != nullptr;
}

bool sqlite_connection::is_valid() const {
  return sqlite3_errcode(db_) == SQLITE_OK;
}

version sqlite_connection::client_version() const {
  return {
    static_cast<unsigned int>(SQLITE_VERSION_NUMBER / 1000000),
    static_cast<unsigned int>((SQLITE_VERSION_NUMBER % 1000000) / 1000),
    static_cast<unsigned int>(SQLITE_VERSION_NUMBER % 1000)
  };
}

version sqlite_connection::server_version() const {
  return {
    static_cast<unsigned int>(SQLITE_VERSION_NUMBER / 100000),
    static_cast<unsigned int>((SQLITE_VERSION_NUMBER % 100000) / 1000),
    static_cast<unsigned int>(SQLITE_VERSION_NUMBER % 1000)
  };
}

int sqlite_connection::parse_result(void *param, int column_count, char **values, char **columns) {
  auto *context = static_cast<fetch_context *>(param);

  sqlite_result_reader::columns column;
  for (int i = 0; i < column_count; ++i) {
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
    for (int i = 0; i < column_count; ++i) {
      context->prototype.emplace_back(columns[i]);
    }
  }

  return 0;
}

utils::result<sqlite_connection::fetch_context, sql::sql_error> sqlite_connection::fetch_internal(const std::string &stmt) const {
  fetch_context context;
  char *errmsg = nullptr;

  if (const int ret = sqlite3_exec(db_, stmt.c_str(), parse_result, &context, &errmsg); ret != SQLITE_OK && ret != SQLITE_DONE) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, std::to_string(ret), sqlite3_errmsg(db_), "sqlite3", stmt});
  }

  return utils::ok(context);
}

utils::result<size_t, sql::sql_error> sqlite_connection::execute(const std::string &stmt) {
  char *errmsg = nullptr;

  if (const int ret = sqlite3_exec(db_, stmt.c_str(), nullptr, nullptr, &errmsg); ret != SQLITE_OK && ret != SQLITE_DONE) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, std::to_string(ret), sqlite3_errmsg(db_), "sqlite3", stmt});
  }

  return utils::ok(static_cast<size_t>(sqlite3_changes(db_)));
}

utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> sqlite_connection::fetch(const sql::query_context& context)
{
  return fetch_internal(context.sql).and_then(
    [&context](const auto &ctx) {
      return utils::ok(std::make_unique<sql::query_result_impl>(
        std::make_unique<sqlite_result_reader>(std::move(ctx.rows), context.prototype.size()),
        context.prototype));
  });
}

utils::result<std::unique_ptr<sql::statement_impl>, sql::sql_error> sqlite_connection::prepare(sql::query_context query) {
  sqlite3_stmt *stmt{};
  const int ret = sqlite3_prepare_v2(db_, query.sql.c_str(), static_cast<int>(query.sql.size()), &stmt, nullptr);
  if (is_not_ok_or_done(ret)) {
    return utils::error(make_error(sql::sql_error_code::PREPARE_FAILED, ret, db_, query.sql));
  }

  std::unique_ptr<sql::statement_impl> s(std::make_unique<sqlite_statement>(db_, stmt, query));
  return utils::ok(std::move(s));
}

data_type string2type(const char *type) {
  if (strncmp(type, "INTEGER", 7) == 0) {
    return data_type::type_int;
  }
  if (strncmp(type, "TINYINT", 7) == 0) {
    return data_type::type_char;
  }
  if (strncmp(type, "SMALLINT", 8) == 0) {
    return data_type::type_short;
  }
  if (strncmp(type, "BIGINT", 6) == 0) {
    return data_type::type_long_long;
  }
  if (strcmp(type, "BOOLEAN") == 0) {
    return data_type::type_bool;
  }
  if (strcmp(type, "REAL") == 0) {
    return data_type::type_double;
  }
  if (strcmp(type, "FLOAT") == 0) {
    return data_type::type_float;
  }
  if (strcmp(type, "DOUBLE") == 0) {
    return data_type::type_double;
  }
  if (strcmp(type, "BLOB") == 0) {
    return data_type::type_blob;
  }
  if (strcmp(type, "NULL") == 0) {
    return data_type::type_null;
  }
  if (strncmp(type, "VARCHAR", 7) == 0) {
    return data_type::type_varchar;
  }
  if (strcmp(type, "DATE") == 0) {
    return data_type::type_date;
  }
  if (strcmp(type, "DATETIME") == 0) {
    return data_type::type_time;
  }
  if (strcmp(type, "TEXT") == 0) {
    return data_type::type_text;
  }
  return data_type::type_unknown;
}

utils::result<std::vector<sql::column_definition>, sql::sql_error> sqlite_connection::describe(const std::string &table) {
  const auto result = fetch_internal("PRAGMA table_info(" + table + ")");
  if (!result.is_ok()) {
    return utils::error(result.err());
  }

  sqlite_result_reader reader(result->rows, result->prototype.size());
  std::vector<sql::column_definition> prototype;
  while(auto fetched = reader.fetch()) {
    if (!fetched.is_ok()) {
      return utils::error(fetched.release_error());
    }
    if (!*fetched) {
      break;
    }

    char *end = nullptr;
    // Todo: add index to column
    auto index = strtoul(reader.column(0), &end, 10);
    std::string name = reader.column(1);

    auto type = (string2type(reader.column(2)));
    end = nullptr;
    auto null_opt{sql::null_option::NULLABLE};
    if (strtoul(reader.column(3), &end, 10) == 0) {
      null_opt = sql::null_option::NOT_NULL;
    }
    // f.default_value(res->column(4));
    prototype.emplace_back(name, type, utils::null_attributes, null_opt, index);
  }

  return utils::ok(prototype);
}

utils::result<bool, sql::sql_error> sqlite_connection::exists(const std::string &/*schema_name*/, const std::string &table_name) {
  const auto result = fetch_internal(
    "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND tbl_name='" + table_name + "' LIMIT 1");
  if (!result.is_ok()) {
    return utils::error(result.err());
  }

  sqlite_result_reader reader(result->rows, result->prototype.size());

  auto fetched = reader.fetch();
  if (!fetched.is_ok()) {
    return utils::error(fetched.release_error());
  }
  if (!*fetched) {
    return utils::error(sql::sql_error{sql::sql_error_code::INVALID_QUERY, "", "", ""});
  }

  int v{};
  reader.read_value(nullptr, 0, v);

  return utils::ok(v == 1);
}

std::string sqlite_connection::to_escaped_string( const utils::blob& value ) const
{
  return utils::to_string(value);
}

}

extern "C" {
MATADOR_SQLITE_API matador::sql::connection_impl *create_database(const matador::sql::connection_info &info) {
  return new matador::backends::sqlite::sqlite_connection(info);
}

MATADOR_SQLITE_API void destroy_database(matador::sql::connection_impl *db) {
  delete db;
}
}
