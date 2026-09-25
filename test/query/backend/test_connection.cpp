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

utils::result<void, utils::error> test_connection::open() {
  is_open_ = true;
  const auto user = info().user;
  return utils::ok<void>();
}

utils::result<void, utils::error> test_connection::close() {
  is_open_ = false;
  return utils::ok<void>();
}

utils::result<bool, utils::error> test_connection::is_open() const {
  return utils::ok(is_open_);
}

utils::result<bool, utils::error> test_connection::is_valid() const {
  return is_open();
}

utils::result<utils::version, utils::error> test_connection::client_version() const {
  return utils::ok(utils::version{1, 2, 3});
}

utils::result<utils::version, utils::error> test_connection::server_version() const {
  return utils::ok(utils::version{3, 2, 1});
}

utils::result<query::execute_result, utils::error> test_connection::execute(const query::query_context &/*context*/) {
  return utils::ok(query::execute_result{4});
}

utils::result<std::unique_ptr<query::query_result_impl>, utils::error> test_connection::fetch(const query::query_context &context) {
  return utils::ok(std::make_unique<query::query_result_impl>(std::make_unique<test_result_reader>(),
                                                            context.columns,
                                                            context.resolver,
                                                            context.result_type,
                                                            context.columns.size()));
}

utils::result<std::unique_ptr<query::statement_impl>, utils::error> test_connection::prepare(const query::query_context &context) {
  std::unique_ptr<query::statement_impl> s(std::make_unique<test_statement>(context));
  return utils::ok(std::move(s));
}

utils::result<std::vector<query::column>, utils::error> test_connection::describe(const std::string &/*table*/) {
  return utils::ok(std::vector<query::column>{});
}

utils::result<bool, utils::error> test_connection::exists(const std::string &/*schema_name*/,
                                                          const std::string &/*table_name*/) {
  return utils::ok(false);
}

utils::result<bool, utils::error> test_connection::sequence_exists(const std::string& /*schema_name*/,
                                                                   const std::string& /*sequence_name*/) {
  return utils::ok(false);
}

std::string test_connection::to_escaped_string(const utils::blob_type_t &value) const {
  return utils::to_string(value);
}
}
