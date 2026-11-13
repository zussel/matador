#include "postgres_connection.hpp"
#include "postgres_error.hpp"
#include "postgres_result_reader.hpp"
#include "postgres_statement.hpp"

#include "matador/query/model/column.hpp"

#include "matador/query/internal/query_result_impl.hpp"
#include "matador/query/model/record.hpp"
#include "matador/query/runtime/error_code.hpp"
#include "matador/query/schema/table_generator.hpp"

#include "matador/utils/string.hpp"

#include <algorithm>

namespace matador::backends::postgres {
postgres_connection::hash_to_string_map postgres_connection::statement_name_map_{};

postgres_connection::postgres_connection(const query::connection_info &info)
: connection_impl(info) {
}

result<void, error> postgres_connection::open() {
  if (conn_ != nullptr) {
    return ok<void>();
  }

  const std::string connection(
    "user=" + info().user + " password=" + info().password + " host=" + info().hostname + " dbname=" + info().database +
    " port=" + std::to_string(info().port));

  conn_ = PQconnectdb(connection.c_str());
  if (PQstatus(conn_) == CONNECTION_BAD) {
    const std::string msg = PQerrorMessage(conn_);
    PQfinish(conn_);
    conn_ = nullptr;
    return failure(make_error(query::error_code::OpenError, nullptr, conn_, "Failed to connect"));
  }

  return ok<void>();
}

result<void, error> postgres_connection::close() {
  if (conn_) {
    PQfinish(conn_);
    conn_ = nullptr;
  }

  return ok<void>();
}

result<bool, error> postgres_connection::is_open() const {
  return ok(conn_ != nullptr);
}

result<bool, error> postgres_connection::is_valid() const {
  return ok(PQstatus(conn_) == CONNECTION_OK);
}

result<utils::version, error> postgres_connection::client_version() const {
  const auto client_version = PQlibVersion();
  return ok(utils::version{
    static_cast<unsigned int>(client_version / 10000),
    static_cast<unsigned int>((client_version % 10000) / 100),
    static_cast<unsigned int>(client_version % 100)
  });
}

result<utils::version, error> postgres_connection::server_version() const {
  const auto server_version = PQserverVersion(conn_);

  if (server_version == 0) {
    return failure(make_error(query::error_code::Failure, nullptr, conn_, "Failed to get server version"));
  }

  return ok(utils::version{
    static_cast<unsigned int>(server_version / 10000),
    static_cast<unsigned int>((server_version % 10000) / 100),
    static_cast<unsigned int>(server_version % 100)
  });
}

static utils::basic_type oid2type(Oid oid);

result<std::unique_ptr<query::query_result_impl>, error> postgres_connection::fetch(const query::query_context &ctx) {
  PGresult *res = PQexec(conn_, ctx.sql.c_str());

  if (is_result_error(res)) {
    const auto err = make_error(query::error_code::FetchFailed, res, conn_, "Failed to fetch", ctx.sql);
    PQclear(res);
    return failure(err);
  }

  std::vector<query::column> prototype = ctx.columns;

  const int num_col = PQnfields(res);
  if (prototype.size() != static_cast<size_t>(num_col)) {
    const auto err = make_error(query::error_code::FetchFailed, res, conn_, "Number of received columns doesn't match expected columns.", ctx.sql);
    PQclear(res);
    return failure(err);
  }
  for (int i = 0; i < num_col; ++i) {
    if (prototype.at(i).type() != utils::basic_type::Null) {
      continue;
    }
    const auto type = oid2type(PQftype(res, i));
    // const char *col_name = PQfname(res, i);
    // const auto size = PQfmod(res, i);
    std::ignore = prototype.at(i).type(type);
  }

  return ok(std::make_unique<query::query_result_impl>(std::make_unique<postgres_result_reader>(res),
                                                                    std::move(prototype),
                                                                    ctx.resolver,
                                                                    ctx.result_type));
}

std::string postgres_connection::generate_statement_name(const query::query_context &ctx) {
  auto it = statement_name_map_.find(ctx.sql_hash);

  if (it == statement_name_map_.end()) {
    it = statement_name_map_.insert(std::make_pair(ctx.sql_hash, utils::to_hex_string(ctx.sql_hash))).first;
  }

  return it->second;
}

result<query::execute_result, error> postgres_connection::execute(const std::string& stmt) const {
    PGresult *res = PQexec(conn_, stmt.c_str());

    if (const auto status = PQresultStatus(res); status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        return failure(make_error(query::error_code::Failure, res, conn_, "Failed to execute", stmt));
    }

    const size_t affected_rows = utils::to<size_t>(PQcmdTuples(res));

    PQclear(res);

    return ok(query::execute_result{affected_rows});
}

result<std::unique_ptr<query::statement_impl>, error> postgres_connection::prepare(const query::query_context &context) {
  auto statement_name = generate_statement_name(context);

  PGresult *result = PQprepare(conn_, statement_name.c_str(), context.sql.c_str(), static_cast<int>(context.bind_vars.size()), nullptr);

  if (is_result_error(result)) {
    return failure(make_error(query::error_code::PrepareFailed, result, conn_, "Failed to prepare", context.sql));
  }

  std::unique_ptr<query::statement_impl> s(std::make_unique<postgres_statement>(conn_, result, statement_name, context));
  return ok(std::move(s));
}

result<query::execute_result, error> postgres_connection::execute(const query::query_context &context) {
  return execute(context.sql);
}

utils::basic_type oid2type(const Oid oid) {
  switch (oid) {
    case 16:
      return utils::basic_type::Boolean;
    case 17:
      return utils::basic_type::Blob;
    case 18:
      return utils::basic_type::Int8;
    case 21:
      return utils::basic_type::Int16;
    case 23:
      return utils::basic_type::Int32;
    case 20:
      return utils::basic_type::Int64;
    case 25:
      return utils::basic_type::Text;
    case 1043:
      return utils::basic_type::Varchar;
    case 700:
      return utils::basic_type::Float;
    case 701:
      return utils::basic_type::Double;
    case 1082:
      return utils::basic_type::Date;
    case 1083:
      return utils::basic_type::Time;
    case 1114:
      return utils::basic_type::DateTime;
    default:
      return utils::basic_type::Null;
  }
}

static utils::basic_type string2type(const char *type) {
  if (strcmp(type, "int2") == 0) {
    return utils::basic_type::Int16;
  }
  if (strcmp(type, "int4") == 0) {
    return utils::basic_type::Int32;
  }
  if (strcmp(type, "int8") == 0) {
    return utils::basic_type::Int64;
  }
  if (strcmp(type, "bool") == 0) {
    return utils::basic_type::Boolean;
  }
  if (strcmp(type, "date") == 0) {
    return utils::basic_type::Date;
  }
  if (strcmp(type, "time") == 0) {
    return utils::basic_type::Time;
  }
  if (strcmp(type, "timestamp") == 0) {
    return utils::basic_type::DateTime;
  }
  if (strcmp(type, "float4") == 0) {
    return utils::basic_type::Float;
  }
  if (strcmp(type, "float8") == 0) {
    return utils::basic_type::Double;
  }
  if (strncmp(type, "varchar", 7) == 0) {
    return utils::basic_type::Varchar;
  }
  if (strcmp(type, "character varying") == 0) {
    return utils::basic_type::Varchar;
  }
  if (strcmp(type, "text") == 0) {
    return utils::basic_type::Text;
  }
  if (strcmp(type, "bytea") == 0) {
    return utils::basic_type::Blob;
  }

  return utils::basic_type::Null;
}

result<std::vector<query::column>, error> postgres_connection::describe(const std::string &table) {
  const std::string stmt(
    "SELECT ordinal_position, column_name, udt_name, data_type, is_nullable, column_default FROM information_schema.columns WHERE table_schema='public' AND table_name='"
    + table + "'");

  PGresult *res = PQexec(conn_, stmt.c_str());

  if (is_result_error(res)) {
    return failure(make_error(query::error_code::DescribeFailed, res, conn_, "Failed to describe", stmt));
  }

  postgres_result_reader reader(res);
  std::vector<query::column> prototype;
  while (auto fetched = reader.fetch()) {
    if (!fetched.is_ok()) {
      return failure(fetched.release_error());
    }
    if (!*fetched) {
      break;
    }
    char *end = nullptr;
    // Todo: Handle error
    // auto index = strtoul(reader.column(0), &end, 10) - 1;
    std::string name = reader.column(1);

    // Todo: extract size
    auto type = (string2type(reader.column(2)));
    end = nullptr;
    query::column_constraints constraints;
    if (strtoul(reader.column(4), &end, 10) == 0) {
      constraints.set(column_constraint::NotNull);
    }
    // f.default_value(res->column(4));
    prototype.emplace_back(query::column::make_plain(name, "", type, constraints));
  }

  return ok(prototype);
}

result<bool, error> postgres_connection::exists(const std::string &schema_name,
                                                              const std::string &table_name) {
  std::string stmt("SELECT 1 FROM information_schema.tables WHERE table_name = '" + table_name + "'");
  if (!schema_name.empty()) {
    stmt += " AND table_schema = '" + schema_name + "'";
  }

  PGresult *res = PQexec(conn_, stmt.c_str());

  if (is_result_error(res)) {
    return failure(make_error(query::error_code::TableExistsFailed, res, conn_, "Failed check if table exists", stmt));
  }

  const auto result = utils::to<size_t>(PQcmdTuples(res));
  if (!result) {
    PQclear(res);
    return failure(make_error(query::error_code::Failure, res, conn_, "Failed to convert result value", stmt));
  }

  PQclear(res);
  return ok(*result == 1);
}

result<bool, error> postgres_connection::sequence_exists(const std::string& schema_name, const std::string& sequence_name) {
    const std::string sql{"SELECT to_regclass('" + schema_name + "." + sequence_name + "')"};
    PGresult* res = PQexec(conn_, sql.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        return failure(make_error(query::error_code::SequenceExistsFailed, res, conn_, "Failed check if sequence exists", sql));
    }

    const bool exists = PQgetisnull(res, 0, 0) ? false : true;
    PQclear(res);

    return ok(exists);
}

std::string postgres_connection::to_escaped_string(const utils::blob_type_t &value) const {
  size_t escapedDataLength;
  unsigned char *escapedData = PQescapeByteaConn(conn_, value.data(), value.size(), &escapedDataLength);

  return {reinterpret_cast<char *>(escapedData), escapedDataLength - 1};
}
}

extern "C" {
MATADOR_POSTGRES_EXPORT matador::query::connection_impl *create_database(const matador::query::connection_info &info) {
  return new matador::backends::postgres::postgres_connection(info);
}

MATADOR_POSTGRES_EXPORT void destroy_database(matador::query::connection_impl *db) {
  delete db;
}
}
