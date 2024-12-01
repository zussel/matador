#include "postgres_connection.hpp"
#include "postgres_error.hpp"
#include "postgres_result_reader.hpp"
#include "postgres_statement.hpp"

#include "matador/sql/record.hpp"

#include <sstream>

namespace matador::backends::postgres {
postgres_connection::string_to_int_map postgres_connection::statement_name_map_{};

postgres_connection::postgres_connection(const sql::connection_info &info)
  : connection_impl(info) {
}

void postgres_connection::open() {
  if (is_open()) {
    return;
  }

  const std::string connection(
    "user=" + info().user + " password=" + info().password + " host=" + info().hostname + " dbname=" + info().database +
    " port=" + std::to_string(info().port));

  conn_ = PQconnectdb(connection.c_str());
  if (PQstatus(conn_) == CONNECTION_BAD) {
    const std::string msg = PQerrorMessage(conn_);
    PQfinish(conn_);
    conn_ = nullptr;
    throw_postgres_error(msg.c_str(), "postgres");
  }
}

void postgres_connection::close() {
  if (conn_) {
    PQfinish(conn_);
    conn_ = nullptr;
  }
}

bool postgres_connection::is_open() const {
  return conn_ != nullptr;
}

bool postgres_connection::is_valid() const {
  return PQstatus(conn_) == CONNECTION_OK;
}

version postgres_connection::client_version() const {
  const auto client_version = PQlibVersion();
  return {
    static_cast<unsigned int>(client_version / 10000),
    static_cast<unsigned int>((client_version % 10000) / 100),
    static_cast<unsigned int>(client_version % 100)
  };
}

version postgres_connection::server_version() const {
  const auto server_version = PQserverVersion(conn_);

  if (server_version == 0) {
    throw_postgres_error("not connected", "postgresql");
  }

  return {
    static_cast<unsigned int>(server_version / 10000),
    static_cast<unsigned int>((server_version % 10000) / 100),
    static_cast<unsigned int>(server_version % 100)
  };
}

utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> postgres_connection::fetch(sql::query_context &&context) {
  PGresult *res = PQexec(conn_, context.sql.c_str());

  if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK &&
                                               status != PGRES_TUPLES_OK) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, PQresultErrorField(res, PG_DIAG_SQLSTATE), PQerrorMessage(conn_), "postgres", context.sql});
  }

//  std::vector<sql::column_definition> prototype;
//  const auto num_col = PQnfields(res);
//  for (int i = 0; i < num_col; ++i) {
//    const char *col_name = PQfname(res, i);
//    auto type = PQftype(res, i);
//    auto size = PQfmod(res, i);
//    prototype.emplace_back(col_name);
//  }

  return utils::ok(std::make_unique<sql::query_result_impl>(std::make_unique<postgres_result_reader>(res), context.prototype));
}

std::string postgres_connection::generate_statement_name(const sql::query_context &query) {
  std::stringstream name;
  name << query.table.name << "_" << query.command_name;
  auto result = postgres_connection::statement_name_map_.find(name.str());

  if (result == postgres_connection::statement_name_map_.end()) {
    result = postgres_connection::statement_name_map_.insert(std::make_pair(name.str(), 0)).first;
  }

  name << "_" << ++result->second;

  return name.str();
}

utils::result<std::unique_ptr<sql::statement_impl>, sql::sql_error> postgres_connection::prepare(sql::query_context context) {
  auto statement_name = postgres_connection::generate_statement_name(context);

  PGresult *result = PQprepare(conn_, statement_name.c_str(), context.sql.c_str(),
                               static_cast<int>(context.bind_vars.size()), nullptr);

  throw_postgres_error(result, conn_, "postgres", context.sql);

  std::unique_ptr<sql::statement_impl> s(std::make_unique<postgres_statement>(conn_, result, statement_name, std::move(context)));
  return utils::ok(std::move(s));
}

utils::result<size_t, sql::sql_error> postgres_connection::execute(const std::string &stmt) {
  PGresult *res = PQexec(conn_, stmt.c_str());

  if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK &&
                                               status != PGRES_TUPLES_OK) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, PQresultErrorField(res, PG_DIAG_SQLSTATE), PQerrorMessage(conn_), "postgres", stmt});
  }

  const auto affected_rows = utils::to_long_long(PQcmdTuples(res));

  PQclear(res);

  return utils::ok(static_cast<size_t>(affected_rows));
}

data_type string2type(const char *type) {
  if (strcmp(type, "int2") == 0) {
    return data_type::type_short;
  } else if (strcmp(type, "int4") == 0) {
    return data_type::type_int;
  } else if (strcmp(type, "int8") == 0) {
    return data_type::type_long_long;
  } else if (strcmp(type, "bool") == 0) {
    return data_type::type_bool;
  } else if (strcmp(type, "date") == 0) {
    return data_type::type_date;
  } else if (strcmp(type, "timestamp") == 0) {
    return data_type::type_time;
  } else if (strcmp(type, "float4") == 0) {
    return data_type::type_float;
  } else if (strcmp(type, "float8") == 0) {
    return data_type::type_double;
  } else if (strncmp(type, "varchar", 7) == 0) {
    return data_type::type_varchar;
  } else if (strcmp(type, "character varying") == 0) {
    return data_type::type_varchar;
  } else if (strcmp(type, "text") == 0) {
    return data_type::type_text;
  } else if (strcmp(type, "bytea") == 0) {
    return data_type::type_blob;
  } else {
    return data_type::type_unknown;
  }
}

utils::result<std::vector<sql::column_definition>, sql::sql_error> postgres_connection::describe(const std::string &table) {
  const std::string stmt(
    "SELECT ordinal_position, column_name, udt_name, data_type, is_nullable, column_default FROM information_schema.columns WHERE table_schema='public' AND table_name='"
    + table + "'");

  PGresult *res = PQexec(conn_, stmt.c_str());

  throw_postgres_error(res, conn_, "postgres", stmt);

  postgres_result_reader reader(res);
  std::vector<sql::column_definition> prototype;
  while (auto fetched = reader.fetch()) {
    if (!fetched.is_ok()) {
      return utils::error(fetched.release_error());
    }
    if (!*fetched) {
      break;
    }
    char *end = nullptr;
    // Todo: Handle error
    auto index = strtoul(reader.column(0), &end, 10) - 1;
    std::string name = reader.column(1);

    // Todo: extract size
    auto type = (string2type(reader.column(2)));
    end = nullptr;
    sql::null_option null_opt{sql::null_option::NULLABLE};
    if (strtoul(reader.column(4), &end, 10) == 0) {
      null_opt = sql::null_option::NOT_NULL;
    }
    // f.default_value(res->column(4));
    prototype.emplace_back(name, type, utils::null_attributes, null_opt, index);
  }

  return utils::ok(prototype);
}

utils::result<bool, sql::sql_error> postgres_connection::exists(const std::string &schema_name, const std::string &table_name) {
  const std::string stmt(
    "SELECT 1 FROM information_schema.tables WHERE table_schema = '" + schema_name + "' AND table_name = '" + table_name
    + "'");

  PGresult *res = PQexec(conn_, stmt.c_str());

  if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK &&
                                               status != PGRES_TUPLES_OK) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, PQresultErrorField(res, PG_DIAG_SQLSTATE), PQerrorMessage(conn_), "postgres", stmt});
  }

  return utils::ok(utils::to_long_long(PQcmdTuples(res)) == 1);
}

std::string postgres_connection::to_escaped_string(const utils::blob& value) const
{
  size_t escapedDataLength;
  unsigned char *escapedData = PQescapeByteaConn(conn_, value.data(), value.size(), &escapedDataLength);

  return {reinterpret_cast<char*>(escapedData), escapedDataLength-1};
}

}

extern "C" {
MATADOR_POSTGRES_API matador::sql::connection_impl *create_database(const matador::sql::connection_info &info) {
  return new matador::backends::postgres::postgres_connection(info);
}

MATADOR_POSTGRES_API void destroy_database(matador::sql::connection_impl *db) {
  delete db;
}
}
