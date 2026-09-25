#include "test_statement.hpp"
#include "test_result_reader.hpp"
#include "test_parameter_binder.hpp"

#include <chrono>
#include <random>
#include <thread>

namespace matador::test::orm {
test_statement::test_statement(const query::query_context &query)
: statement_impl(query, 0) {}

utils::result<query::execute_result, utils::error> test_statement::execute(const query::value_writer &/*bindings*/) {
  using namespace std::chrono_literals;
  std::mt19937 rng(query_.sql.size());
  std::uniform_int_distribution dist(10, 50);
  std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
  return utils::ok(query::execute_result{8});
}

utils::result<std::unique_ptr<query::query_result_impl>, utils::error> test_statement::fetch(const query::value_writer &/*bindings*/) {
  return utils::ok(std::make_unique<query::query_result_impl>(std::make_unique<test_result_reader>(),
    query_.columns,
    query_.resolver,
    query_.result_type,
    query_.columns.size()));
}

void test_statement::reset() {}
std::unique_ptr<query::value_writer> test_statement::create_binder() const {
  return std::make_unique<test_parameter_binder>();
}

} // namespace matador::test::orm
