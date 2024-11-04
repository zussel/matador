#ifndef QUERY_POSTGRES_CONNECTION_HPP
#define QUERY_POSTGRES_CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_mysql_EXPORTS
#define MATADOR_MYSQL_API __declspec(dllexport)
#else
#define MATADOR_MYSQL_API __declspec(dllimport)
#endif
#pragma warning(disable: 4355)
#else
#define MATADOR_MYSQL_API
#endif

#include "matador/sql/connection_impl.hpp"

#include <unordered_map>

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace matador::backends::mysql {

class mysql_connection : public matador::sql::connection_impl
{
public:
  explicit mysql_connection(const sql::connection_info &info);
  void open() override;
  void close() override;
  [[nodiscard]] bool is_open() const override;
  [[nodiscard]] bool is_valid() const override;
  [[nodiscard]] version client_version() const override;
  [[nodiscard]] version server_version() const override;

  utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> fetch(const sql::query_context &context) override;
  utils::result<std::unique_ptr<sql::statement_impl>, sql::sql_error> prepare(sql::query_context context) override;
  utils::result<size_t, sql::sql_error> execute(const std::string &stmt) override;
  utils::result<std::vector<sql::column_definition>, sql::sql_error> describe(const std::string& table) override;
  utils::result<bool, sql::sql_error> exists(const std::string &schema_name, const std::string &table_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob& value ) const override;

private:
  mutable std::unique_ptr<MYSQL> mysql_;

  using string_to_int_map = std::unordered_map<std::string, unsigned long>;

  static string_to_int_map statement_name_map_;
};

}

extern "C"
{
MATADOR_MYSQL_API matador::sql::connection_impl* create_database(const matador::sql::connection_info &info);

MATADOR_MYSQL_API void destroy_database(matador::sql::connection_impl *db);
}

#endif //QUERY_POSTGRES_CONNECTION_HPP
