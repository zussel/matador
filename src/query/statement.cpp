#include "matador/query/statement.hpp"
#include "matador/query/execute_result.hpp"
#include "matador/query/record.hpp"

#include <algorithm>
#include <utility>

namespace matador::query {

statement::statement(const std::shared_ptr<statement_proxy>& proxy, logger_ptr logger)
: statement_proxy_(proxy)
, bindings_(proxy->create_binder())
, logger_(std::move(logger)){}

statement::statement(statement&& x) noexcept
: statement_proxy_(std::move(x.statement_proxy_))
, bindings_(std::move(x.bindings_))
, logger_(std::move(x.logger_)) {
}

statement& statement::operator=(statement&& x) noexcept {
  statement_proxy_ = std::move(x.statement_proxy_);
  bindings_ = std::move(x.bindings_);
  logger_ = std::move(x.logger_);
  return *this;
}

statement::statement(const statement& x)
: statement_proxy_(x.statement_proxy_)
, bindings_(x.statement_proxy_->create_binder())
, logger_(x.logger_) {
}
statement& statement::operator=(const statement& x) {
  if (&x == this) {
    return *this;
  }
  statement_proxy_ = x.statement_proxy_;
  bindings_ = x.statement_proxy_->create_binder();
  logger_ = x.logger_;
  return *this;
}

statement &statement::bind(const size_t pos, const char *value) {
  statement_proxy_->bind(pos, value, strlen(value), *bindings_);
  return *this;
}

statement &statement::bind(const size_t pos, std::string &val, const size_t size) {
  statement_proxy_->bind(pos, val, size, *bindings_);
  return *this;
}

utils::result<execute_result, utils::error> statement::execute() const {
  logger_->on_execute(statement_proxy_->sql());
  std::cout << statement_proxy_->sql() << std::endl;
  return statement_proxy_->execute(*bindings_);
}

utils::result<query_result<record>, utils::error> statement::fetch() const {
  std::cout << statement_proxy_->sql() << std::endl;
  auto result = statement_proxy_->fetch(*bindings_);
  if (!result.is_ok()) {
    return utils::failure(result.err());
  }
  logger_->on_fetch(statement_proxy_->sql());
  const auto prototype = result.value()->prototype();
  auto fetch_result = query_result<record>::make_query_result(std::move(*result), prototype);
  if (!fetch_result.is_ok()) {
    return failure<error>(fetch_result.err());
  }

  return ok<query_result<record>>(fetch_result.release());
}

utils::result<std::optional<record>, utils::error> statement::fetch_one() const {
  logger_->on_fetch(statement_proxy_->sql());
  std::cout << statement_proxy_->sql() << std::endl;
  auto result = statement_proxy_->fetch(*bindings_);
  if (!result.is_ok()) {
    return utils::failure(result.err());
  }

  const auto prototype = result.value()->prototype();
  auto fetch_result = query_result<record>::make_query_result(std::move(*result), prototype);
  if (!fetch_result.is_ok()) {
    return failure<error>(fetch_result.err());
  }
  auto records = fetch_result.release();
  auto first = records.begin();
  if (first == records.end()) {
    return utils::ok(std::optional<record>{std::nullopt});
  }
  logger_->on_fetch(statement_proxy_->sql());
  return utils::ok(std::optional{std::move(*first)});
}

void statement::reset() const {
  statement_proxy_->reset();
}

size_t statement::bind_pos() const {
  return statement_proxy_->bind_pos();
}

std::string statement::sql() const {
  return statement_proxy_->sql();
}

utils::result<std::unique_ptr<query_result_impl>, utils::error> statement::fetch_internal() const {
  auto result = statement_proxy_->fetch(*bindings_);
  if (!result.is_ok()) {
    return utils::failure(result.err());
  }
  logger_->on_fetch(statement_proxy_->sql());

  return result;
}
}
