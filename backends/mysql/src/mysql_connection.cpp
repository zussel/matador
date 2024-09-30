#include "mysql_connection.hpp"
#include "mysql_error.hpp"
#include "mysql_result_reader.hpp"
#include "mysql_statement.hpp"

#include "matador/sql/record.hpp"
#include "matador/object/data_type_traits.hpp"

#include <memory>
#include <regex>

namespace matador::backends::mysql {
mysql_connection::string_to_int_map mysql_connection::statement_name_map_{};

mysql_connection::mysql_connection(const sql::connection_info &info)
  : connection_impl(info) {
}

void mysql_connection::open() {
  if (is_open()) {
    return;
  }

  mysql_ = std::make_unique<MYSQL>();

  if (!mysql_init(mysql_.get())) {
    throw_mysql_error(mysql_.get(), "mysql_init");
  }

  if (!mysql_real_connect(mysql_.get(),
                          info().hostname.c_str(),
                          info().user.c_str(),
                          !info().password.empty() ? info().password.c_str() : nullptr,
                          info().database.c_str(),
                          info().port,
                          nullptr,
                          0)) {
    // disconnect all handles
    const std::string error_message = mysql_error(mysql_.get());
    mysql_close(mysql_.get());

    mysql_.reset();
    // throw exception
    throw_mysql_error(error_message.c_str(), "mysql_real_connect");
  }
}

void mysql_connection::close() {
  if (mysql_) {
    mysql_close(mysql_.get());
    mysql_.reset();
  }
}

bool mysql_connection::is_open() const {
  return mysql_ != nullptr;
}

bool mysql_connection::is_valid() const {
  // Send a ping to the server to check the connection status
  if (mysql_ping(mysql_.get()) != 0) {
    // Check for errors using mysql_errno()
    if (mysql_errno(mysql_.get()) != 0) {
      std::cerr << "Connection error: " << mysql_error(mysql_.get()) << std::endl;
    }
    return false;
  }

  return true;
}

version mysql_connection::client_version() const {
  const auto client_version = mysql_get_client_version();
  return {
    static_cast<unsigned int>(client_version / 10000),
    static_cast<unsigned int>((client_version % 10000) / 100),
    static_cast<unsigned int>(client_version % 100)
  };
}

version mysql_connection::server_version() const {
  const auto server_version = mysql_get_server_version(mysql_.get());

  if (server_version == 0) {
    throw_mysql_error("not connected", "mysql");
  }

  return {
    static_cast<unsigned int>(server_version / 10000),
    static_cast<unsigned int>((server_version % 10000) / 100),
    static_cast<unsigned int>(server_version % 100)
  };
}

matador::data_type to_type(enum_field_types type, unsigned int flags) {
  switch (type) {
    case MYSQL_TYPE_TINY:
      return flags & UNSIGNED_FLAG ? data_type::type_unsigned_char : data_type::type_char;
    case MYSQL_TYPE_SHORT:
      return flags & UNSIGNED_FLAG ? data_type::type_unsigned_short : data_type::type_short;
    case MYSQL_TYPE_LONG:
      return flags & UNSIGNED_FLAG ? data_type::type_unsigned_int : data_type::type_int;
    case MYSQL_TYPE_LONGLONG:
      return flags & UNSIGNED_FLAG ? data_type::type_unsigned_long_long : data_type::type_long_long;
    case MYSQL_TYPE_FLOAT:
      return data_type::type_float;
    case MYSQL_TYPE_DOUBLE:
      return data_type::type_double;
    case MYSQL_TYPE_VARCHAR:
    case MYSQL_TYPE_VAR_STRING:
      return data_type::type_varchar;
    case MYSQL_TYPE_BLOB:
      return data_type::type_blob;
    case MYSQL_TYPE_STRING:
      return data_type::type_text;
    case MYSQL_TYPE_DATE:
      return data_type::type_date;
    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_TIMESTAMP:
      return data_type::type_time;
    default:
      return data_type::type_unknown;
  }
}

utils::constraints to_constraints(unsigned int flags) {
  utils::constraints options{utils::constraints::NONE};
  if (flags & PRI_KEY_FLAG) {
    options |= utils::constraints::PRIMARY_KEY;
  }
  if (flags & UNIQUE_KEY_FLAG) {
    options |= utils::constraints::UNIQUE;
  }

  return options;
}

sql::null_option to_null_option(unsigned int flags) {
  return flags & NOT_NULL_FLAG ? sql::null_option::NOT_NULL : sql::null_option::NULLABLE;
}

data_type string2type(const std::string &type_string) {
  if (strncmp(type_string.c_str(), "tinyint", 7) == 0) {
    return data_type::type_char;
  } else if (strncmp(type_string.c_str(), "smallint", 8) == 0) {
    if (strstr(type_string.c_str(), "unsigned") != nullptr) {
      return data_type::type_unsigned_short;
    } else {
      return data_type::type_short;
    }
  } else if (strncmp(type_string.c_str(), "int", 3) == 0) {
    if (strstr(type_string.c_str(), "unsigned") != nullptr) {
      return data_type::type_unsigned_int;
    } else {
      return data_type::type_int;
    }
  } else if (strncmp(type_string.c_str(), "bigint", 6) == 0) {
    if (strstr(type_string.c_str(), "unsigned") != nullptr) {
      return data_type::type_unsigned_long_long;
    } else {
      return data_type::type_long_long;
    }
  } else if (strcmp(type_string.c_str(), "date") == 0) {
    return data_type::type_date;
  } else if (strncmp(type_string.c_str(), "datetime", 8) == 0) {
    return data_type::type_time;
  } else if (strcmp(type_string.c_str(), "float") == 0) {
    return data_type::type_float;
  } else if (strcmp(type_string.c_str(), "double") == 0) {
    return data_type::type_double;
  } else if (strncmp(type_string.c_str(), "varchar", 7) == 0) {
    return data_type::type_varchar;
  } else if (strncmp(type_string.c_str(), "text", 4) == 0) {
    return data_type::type_text;
  } else {
    return data_type::type_unknown;
  }
}

struct type_info {
  data_type type{data_type::type_unknown};
  size_t size{};
};

type_info determine_type_info(const std::string &type_string) {
  static const std::regex TYPE_REGEX(R"(^(\w+)(\((\d+)(,(\d+))?\))?$)");
  std::smatch matcher;

  type_info result;
  if (std::regex_match(type_string, matcher, TYPE_REGEX)) {
    result.type = string2type(matcher[1].str());
    if (matcher[3].matched) {
      result.size = std::stoi(matcher[3].str());
    }
  }
  return result;
}

std::unique_ptr<sql::query_result_impl> mysql_connection::fetch(const sql::query_context &context) {
  if (mysql_query(mysql_.get(), context.sql.c_str())) {
    throw_mysql_error(mysql_.get(), context.sql);
  }

  auto result = mysql_store_result(mysql_.get());
  if (result == nullptr) {
    throw_mysql_error(mysql_.get(), context.sql);
  }

  auto field_count = mysql_num_fields(result);
  const auto fields = mysql_fetch_fields(result);
  std::vector<sql::column_definition> prototype;
  for (unsigned i = 0; i < field_count; ++i) {
    auto type = to_type(fields[i].type, fields[i].flags);
    auto options = to_constraints(fields[i].flags);
    auto null_opt = to_null_option(fields[i].flags);

    prototype.emplace_back(fields[i].name, type, options, null_opt);
  }

  return std::move(std::make_unique<sql::query_result_impl>(std::make_unique<mysql_result_reader>(result, field_count),
                                                            std::move(prototype)));
}

std::unique_ptr<sql::statement_impl> mysql_connection::prepare(sql::query_context context) {
  MYSQL_STMT *stmt = mysql_stmt_init(mysql_.get());
  if (stmt == nullptr) {
    throw_mysql_error(mysql_.get(), "mysql_stmt_init");
  }

  if (mysql_stmt_prepare(stmt, context.sql.c_str(), static_cast<unsigned long>(context.sql.size())) != 0) {
    throw_mysql_error(stmt, "mysql_stmt_prepare", context.sql);
  }

  return std::make_unique<mysql_statement>(stmt, std::move(context));
}

size_t mysql_connection::execute(const std::string &stmt) {
  if (mysql_query(mysql_.get(), stmt.c_str())) {
    throw_mysql_error(mysql_.get(), stmt);
  }

  return mysql_affected_rows(mysql_.get());
}

std::vector<sql::column_definition> mysql_connection::describe(const std::string &table) {
  std::string stmt("SHOW COLUMNS FROM " + table);

  if (mysql_query(mysql_.get(), stmt.c_str())) {
    throw_mysql_error(mysql_.get(), stmt);
  }

  auto result = mysql_store_result(mysql_.get());
  if (result == nullptr) {
    throw_mysql_error(mysql_.get(), stmt);
  }

  mysql_result_reader reader(result, mysql_num_fields(result));
  std::vector<sql::column_definition> prototype;
  while (reader.fetch()) {
    char *end = nullptr;
    std::string name = reader.column(0);

    auto typeinfo = determine_type_info(reader.column(1));
    end = nullptr;
    sql::null_option null_opt{sql::null_option::NULLABLE};
    if (strtoul(reader.column(2), &end, 10) == 0) {
      null_opt = sql::null_option::NOT_NULL;
    }
    prototype.push_back({name, typeinfo.type, {typeinfo.size}, null_opt, prototype.size()});
  }

  return prototype;
}

bool mysql_connection::exists(const std::string &/*schema_name*/, const std::string &table_name) {
  const std::string stmt(
    "SELECT 1 FROM information_schema.tables WHERE table_schema = '" + info().database + "' AND table_name = '" +
    table_name + "'");

  if (mysql_query(mysql_.get(), stmt.c_str())) {
    throw_mysql_error(mysql_.get(), stmt);
  }

  const auto result = mysql_store_result(mysql_.get());
  if (result == nullptr) {
    throw_mysql_error(mysql_.get(), stmt);
  }

  return result->row_count == 1;
}

std::string mysql_connection::to_escaped_string( const utils::blob& value ) const
{
    const auto escapedDataSize = value.size() * 2 + 1;  // Maximum size for escaped data
    auto *escapedData = new char[escapedDataSize];

    unsigned long escapedLength = mysql_real_escape_string(
        mysql_.get(),
        escapedData,
        reinterpret_cast<const char *>(value.data()),
        static_cast<unsigned long>(value.size())
    );

    return {escapedData, escapedLength};
}

}

extern "C" {
MATADOR_MYSQL_API matador::sql::connection_impl *create_database(const matador::sql::connection_info &info) {
  return new matador::backends::mysql::mysql_connection(info);
}

MATADOR_MYSQL_API void destroy_database(matador::sql::connection_impl *db) {
  delete db;
}
}
