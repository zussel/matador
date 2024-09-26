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
  size_t execute(const std::string &stmt) override;
  std::unique_ptr<query_result_impl> fetch(const query_context &context) override;
  std::unique_ptr<statement_impl> prepare(query_context context) override;
  std::vector<sql::column_definition> describe(const std::string &table) override;
  bool exists(const std::string &schema_name, const std::string &table_name) override;

private:
  bool is_open_{false};
};
}
#endif //QUERY_NOOP_CONNECTION_HPP
