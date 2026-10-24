#include "test_connection.hpp"
#include "test_statement.hpp"
#include "test_result_reader.hpp"

#include "matador/query/query_context.hpp"

#include "matador/query/internal/query_result_impl.hpp"

#include "matador/query/interface/statement_impl.hpp"

#include "matador/utils/string.hpp"

#include <string>
#include <memory>

namespace matador::test::orm {
test_connection::test_connection(const query::connection_info &info)
: connection_impl(info) {}

result<void, error> test_connection::open() {
  is_open_ = true;
  const auto user = info().user;
  return ok<void>();
}

result<void, error> test_connection::close() {
  is_open_ = false;
  return ok<void>();
}

result<bool, error> test_connection::is_open() const {
  return ok(is_open_);
}

result<bool, error> test_connection::is_valid() const {
  return is_open();
}

result<utils::version, error> test_connection::client_version() const {
  return ok(utils::version{1, 2, 3});
}

result<utils::version, error> test_connection::server_version() const {
  return ok(utils::version{3, 2, 1});
}

result<query::execute_result, error> test_connection::execute(const query::query_context &/*context*/) {
  return ok(query::execute_result{4});
}

result<std::unique_ptr<query::query_result_impl>, error> test_connection::fetch(const query::query_context &context) {
  return ok(std::make_unique<query::query_result_impl>(std::make_unique<test_result_reader>(),
                                                            context.columns,
                                                            context.resolver,
                                                            context.result_type,
                                                            context.columns.size()));
}

result<std::unique_ptr<query::statement_impl>, error> test_connection::prepare(const query::query_context &context) {
  std::unique_ptr<query::statement_impl> s(std::make_unique<test_statement>(context));
  return ok(std::move(s));
}

result<std::vector<query::column>, error> test_connection::describe(const std::string &/*table*/) {
  return ok(std::vector<query::column>{});
}

result<bool, error> test_connection::exists(const std::string &/*schema_name*/,
                                                          const std::string &/*table_name*/) {
  return ok(false);
}

result<bool, error> test_connection::sequence_exists(const std::string& /*schema_name*/,
                                                                   const std::string& /*sequence_name*/) {
  return ok(false);
}

std::string test_connection::to_escaped_string(const utils::blob_type_t &value) const {
  return utils::to_string(value);
}
}
