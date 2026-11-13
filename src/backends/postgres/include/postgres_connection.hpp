#ifndef QUERY_POSTGRES_CONNECTION_HPP
#define QUERY_POSTGRES_CONNECTION_HPP

#include "src/backends/postgres/matador-postgres_export.h"

#include "matador/query/interface/connection_impl.hpp"

#include <unordered_map>

#include <libpq-fe.h>

namespace matador::backends::postgres {

class postgres_connection : public query::connection_impl
{
public:
  explicit postgres_connection(const query::connection_info &info);
  result<void, error> open() override;
  result<void, error> close() override;
  [[nodiscard]] result<bool, error> is_open() const override;
  [[nodiscard]] result<bool, error> is_valid() const override;
  [[nodiscard]] result<utils::version, error> client_version() const override;
  [[nodiscard]] result<utils::version, error> server_version() const override;

  result<query::execute_result, error> execute(const query::query_context &context) override;
  result<std::unique_ptr<query::statement_impl>, error> prepare(const query::query_context &context) override;
  result<std::unique_ptr<query::query_result_impl>, error> fetch(const query::query_context &ctx) override;

  result<std::vector<query::column>, error> describe(const std::string& table) override;
  result<bool, error> exists(const std::string &schema_name, const std::string &table_name) override;

  result<bool, error> sequence_exists(const std::string &schema_name, const std::string &sequence_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob_type_t& value ) const override;

private:
  [[nodiscard]] static std::string generate_statement_name(const query::query_context &ctx) ;

  [[nodiscard]] result<query::execute_result, error> execute(const std::string &stmt) const;

private:
  PGconn *conn_{nullptr};

  using hash_to_string_map = std::unordered_map<size_t, std::string>;

  static hash_to_string_map statement_name_map_;
};

}

extern "C"
{
MATADOR_POSTGRES_EXPORT matador::query::connection_impl* create_database(const matador::query::connection_info &info);

MATADOR_POSTGRES_EXPORT void destroy_database(matador::query::connection_impl *db);
}

#endif //QUERY_POSTGRES_CONNECTION_HPP
