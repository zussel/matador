#ifndef QUERY_NOOP_CONNECTION_HPP
#define QUERY_NOOP_CONNECTION_HPP

#include "matador/sql/connection_impl.hpp"

namespace matador::sql {

class noop_connection final : public connection_impl
{
public:
  explicit noop_connection(const connection_info &info);

  void open() override;
  void close() override;
  [[nodiscard]] bool is_open() const override;
  [[nodiscard]] bool is_valid() const override;
  [[nodiscard]] version client_version() const override;
  [[nodiscard]] version server_version() const override;
  utils::result<size_t, sql_error> execute(const std::string &stmt) override;
  utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> fetch(const query_context &context) override;
  utils::result<std::unique_ptr<statement_impl>, sql_error> prepare(query_context context) override;
  utils::result<std::vector<sql::column_definition>, sql_error> describe(const std::string &table) override;
  utils::result<bool, sql_error> exists(const std::string &schema_name, const std::string &table_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob& value ) const override;

private:
  bool is_open_{false};
};
}
#endif //QUERY_NOOP_CONNECTION_HPP
