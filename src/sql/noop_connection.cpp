#include "matador/sql/noop_connection.hpp"
#include "matador/sql/query_context.hpp"
#include "matador/sql/record.hpp"

#include "matador/utils/string.hpp"

#include <string>
#include <memory>

namespace matador::sql {
noop_connection::noop_connection(const connection_info &info)
: connection_impl(info) {}

void noop_connection::open()
{
  is_open_ = true;
}

void noop_connection::close()
{
  is_open_ = false;
}

bool noop_connection::is_open() const
{
  return is_open_;
}

bool noop_connection::is_valid() const
{
  return is_open();
}

version noop_connection::client_version() const {
  return {};
}

version noop_connection::server_version() const {
  return {};
}

utils::result<size_t, sql_error> noop_connection::execute(const std::string &/*stmt*/)
{
  return utils::ok(static_cast<size_t>(0));
}

utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> noop_connection::fetch(const query_context &/*context*/)
{
  return utils::ok(std::unique_ptr<sql::query_result_impl>{});
}

utils::result<std::unique_ptr<statement_impl>, sql_error> noop_connection::prepare(query_context /*context*/)
{
  return utils::ok(std::unique_ptr<statement_impl>{});
}

utils::result<std::vector<sql::column_definition>, sql_error> noop_connection::describe(const std::string &/*table*/)
{
  return utils::ok(std::vector<sql::column_definition>{});
}

utils::result<bool, sql_error> noop_connection::exists(const std::string &/*schema_name*/, const std::string &/*table_name*/)
{
  return utils::ok(false);
}

std::string noop_connection::to_escaped_string(const utils::blob& value) const
{
  return utils::to_string(value);
}

}