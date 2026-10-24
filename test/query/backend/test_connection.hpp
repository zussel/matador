#ifndef QUERY_NOOP_CONNECTION_HPP
#define QUERY_NOOP_CONNECTION_HPP

#include "matador/query/interface/connection_impl.hpp"
#include "matador/query/execute_result.hpp"

namespace matador::test::orm {

class test_connection final : public query::connection_impl {
public:
  explicit test_connection(const query::connection_info &info);

  result<void, error> open() override;
  result<void, error> close() override;
  [[nodiscard]] result<bool, error> is_open() const override;
  [[nodiscard]] result<bool, error> is_valid() const override;
  [[nodiscard]] result<utils::version, error> client_version() const override;
  [[nodiscard]] result<utils::version, error> server_version() const override;
  result<query::execute_result, error> execute(const query::query_context &context) override;
  result<std::unique_ptr<query::query_result_impl>, error> fetch(const query::query_context &context) override;
  result<std::unique_ptr<query::statement_impl>, error> prepare(const query::query_context &context) override;
  result<std::vector<query::column>, error> describe(const std::string &table) override;
  result<bool, error> exists(const std::string &schema_name, const std::string &table_name) override;
  result<bool, error> sequence_exists(const std::string &schema_name, const std::string &sequence_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob_type_t& value ) const override;

private:
  bool is_open_{false};
};

}
#endif //QUERY_NOOP_CONNECTION_HPP
