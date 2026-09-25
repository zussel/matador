#ifndef QUERY_NOOP_CONNECTION_HPP
#define QUERY_NOOP_CONNECTION_HPP

#include "matador/query/interface/connection_impl.hpp"
#include "matador/query/execute_result.hpp"

namespace matador::test::orm {

class test_connection final : public query::connection_impl {
public:
  explicit test_connection(const query::connection_info &info);

  utils::result<void, utils::error> open() override;
  utils::result<void, utils::error> close() override;
  [[nodiscard]] utils::result<bool, utils::error> is_open() const override;
  [[nodiscard]] utils::result<bool, utils::error> is_valid() const override;
  [[nodiscard]] utils::result<utils::version, utils::error> client_version() const override;
  [[nodiscard]] utils::result<utils::version, utils::error> server_version() const override;
  utils::result<query::execute_result, utils::error> execute(const query::query_context &context) override;
  utils::result<std::unique_ptr<query::query_result_impl>, utils::error> fetch(const query::query_context &context) override;
  utils::result<std::unique_ptr<query::statement_impl>, utils::error> prepare(const query::query_context &context) override;
  utils::result<std::vector<query::column>, utils::error> describe(const std::string &table) override;
  utils::result<bool, utils::error> exists(const std::string &schema_name, const std::string &table_name) override;
  utils::result<bool, utils::error> sequence_exists(const std::string &schema_name, const std::string &sequence_name) override;

  [[nodiscard]] std::string to_escaped_string( const utils::blob_type_t& value ) const override;

private:
  bool is_open_{false};
};

}
#endif //QUERY_NOOP_CONNECTION_HPP
