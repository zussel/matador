#ifndef QUERY_SQLITE_CONNECTION_HPP
#define QUERY_SQLITE_CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_sqlite_EXPORTS
#define MATADOR_SQLITE_API __declspec(dllexport)
#else
#define MATADOR_SQLITE_API __declspec(dllimport)
#endif
#pragma warning(disable: 4355)
#else
#define MATADOR_SQLITE_API
#endif

#include "matador/sql/connection_impl.hpp"

#include "sqlite_result_reader.hpp"

#include <sqlite3.h>

namespace matador::backends::sqlite {

class sqlite_connection final : public sql::connection_impl
{
public:
  explicit sqlite_connection(const sql::connection_info &info);
  void open() override;
  void close() override;
  [[nodiscard]] bool is_open() const override;
  [[nodiscard]] bool is_valid() const override;
  [[nodiscard]] version client_version() const override;
  [[nodiscard]] version server_version() const override;

  utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> fetch(sql::query_context &&context) override;
  utils::result<std::unique_ptr<sql::statement_impl>, sql::sql_error> prepare(sql::query_context query) override;
  utils::result<size_t, sql::sql_error> execute(const std::string &stmt) override;
  utils::result<std::vector<sql::column_definition>, sql::sql_error> describe(const std::string& table) override;
  utils::result<bool, sql::sql_error> exists(const std::string &schema_name, const std::string &table_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob& value ) const override;

private:
  struct fetch_context
  {
    std::vector<sql::column_definition> prototype;
    sqlite_result_reader::rows rows;
  };

private:
  static int parse_result(void* param, int column_count, char** values, char** columns);

  [[nodiscard]] utils::result<fetch_context, sql::sql_error> fetch_internal(const std::string &stmt) const;

private:
  sqlite3 *db_{};
};

}

extern "C"
{
MATADOR_SQLITE_API matador::sql::connection_impl* create_database(const matador::sql::connection_info &info);

MATADOR_SQLITE_API void destroy_database(matador::sql::connection_impl *db);
}

#endif //QUERY_SQLITE_CONNECTION_HPP
