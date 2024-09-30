#ifndef QUERY_POSTGRES_CONNECTION_HPP
#define QUERY_POSTGRES_CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_postgres_EXPORTS
#define MATADOR_POSTGRES_API __declspec(dllexport)
#else
#define MATADOR_POSTGRES_API __declspec(dllimport)
#endif
#pragma warning(disable: 4355)
#else
#define MATADOR_POSTGRES_API
#endif

#include "matador/sql/connection_impl.hpp"

#include <unordered_map>

#include <libpq-fe.h>

namespace matador::backends::postgres {

class postgres_connection : public sql::connection_impl
{
public:
  explicit postgres_connection(const sql::connection_info &info);
  void open() override;
  void close() override;
  [[nodiscard]] bool is_open() const override;
  [[nodiscard]] bool is_valid() const override;
  [[nodiscard]] version client_version() const override;
  [[nodiscard]] version server_version() const override;

  std::unique_ptr<sql::query_result_impl> fetch(const sql::query_context &context) override;
  std::unique_ptr<sql::statement_impl> prepare(sql::query_context context) override;

  size_t execute(const std::string &stmt) override;

  std::vector<sql::column_definition> describe(const std::string& table) override;

  bool exists(const std::string &schema_name, const std::string &table_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob& value ) const override;

private:
  [[nodiscard]] static std::string generate_statement_name(const sql::query_context &query) ;

private:
  PGconn *conn_{nullptr};

  using string_to_int_map = std::unordered_map<std::string, unsigned long>;

  static string_to_int_map statement_name_map_;
};

}

extern "C"
{
MATADOR_POSTGRES_API matador::sql::connection_impl* create_database(const matador::sql::connection_info &info);

MATADOR_POSTGRES_API void destroy_database(matador::sql::connection_impl *db);
}

#endif //QUERY_POSTGRES_CONNECTION_HPP
