#ifndef TEST_STATEMENT_HPP
#define TEST_STATEMENT_HPP

#include "matador/query/interface/statement_impl.hpp"
#include "matador/query/execute_result.hpp"

namespace matador::test::orm {

class test_statement final : public query::statement_impl {
public:
  explicit test_statement(const query::query_context &query);
  utils::result<query::execute_result, utils::error> execute(const query::value_writer &bindings) override;
  utils::result<std::unique_ptr<query::query_result_impl>, utils::error> fetch(const query::value_writer &bindings) override;
  void reset() override;

protected:
  [[nodiscard]] std::unique_ptr<query::value_writer> create_binder() const override;
};

}

#endif //TEST_STATEMENT_HPP
