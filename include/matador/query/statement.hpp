#ifndef QUERY_STATEMENT_HPP
#define QUERY_STATEMENT_HPP

#include "matador/query/abstract_sql_logger.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/execute_result.hpp"
#include "matador/query/query_result.hpp"
#include "matador/query/interface/statement_proxy.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <iostream>
#include <memory>


namespace matador::query {

class statement_impl;
class statement_proxy;

class statement {
public:
  using logger_ptr = std::shared_ptr<abstract_sql_logger>;

  /**
   * Creates a statement initialized from the
   * given statement implementation object holding
   * the implementation for the selected database
   *
   * @param proxy The statement proxy implementation object
   * @param logger The logger handler to write SQL log messages to
   */
  explicit statement(const std::shared_ptr<statement_proxy>& proxy, logger_ptr logger = null_logger);
  /**
   * Copy move constructor for statement
   *
   * @param x The statement to move from
   */
  statement(statement &&x) noexcept;

  /**
   * Assignment move constructor for statement
   *
   * @param x The statement to move from
   * @return Reference to this
   */
  statement &operator=(statement &&x) noexcept;

  statement(const statement &x);
  statement &operator=(const statement &x);

  statement &bind(size_t pos, const char *value);
  statement &bind(size_t pos, std::string &val, size_t size);
  /**
   * Bind an object to the statement starting
   * at the given position index.
   *
   * @param obj The object to bind
   * @return The next index to bind
   */
  template<class Type>
  statement &bind(const Type &obj);
  template<class Type>
  statement &bind(const object_ptr<Type> &ptr);
  template<typename Type>
  statement &bind(size_t pos, Type &value);

  /**
   * Executes the prepared statement and returns
   * the number of affected rows.
   *
   * @return The number of affected rows
   */
  [[nodiscard]] utils::result<execute_result, utils::error> execute() const;

  /**
   * Fetches the result of the prepared
   * statement. If a prepared statement was not
   * a SELECT statement, an empty query result set
   * is returned.
   *
   * @tparam Type Type of the fetched result
   * @return The query result set
   */
  template<class Type>
  utils::result<query_result<Type>, utils::error> fetch();
  /**
   * Fetches the result of the prepared
   * statement. The type is the record representing an
   * unknown variable type.
   * If the prepared statement was not
   * a SELECT statement, an empty query result set
   * is returned.
   *
   * @return The query result set
   */
  [[nodiscard]] utils::result<query_result<record>, utils::error> fetch() const;
  /**
   * Fetches the first result of a prepared statement.
   * If a prepared statement is empty or not
   * a SELECT statement, a nullptr is returned.
   *
   * @tparam Type Type of the fetched result
   * @return The query result set
   */
  template<class Type>
  utils::result<object_ptr<Type>, utils::error> fetch_one();

  template<class Type>
  utils::result<std::shared_ptr<Type>, utils::error> fetch_one_raw();
  /**
   * Fetches the first result of a prepared statement.
   * The type is a record representing an unknown variable type.
   * If a prepared statement is empty or not
   * a SELECT statement, a nullptr is returned.
   *
   * @return The query result set
   */
  [[nodiscard]] utils::result<std::optional<record>, utils::error> fetch_one() const;

  /**
   * Resets the prepared statement to
   * reuse it.
   */
  void reset() const;

  /**
   * Returns the current binding position.
   * Is set to the initial position after
   * a call to reset().
   *
   * @return The current binding position
   */
  [[nodiscard]] size_t bind_pos() const;

  /**
   * Returns the statements underlying SQL
   * query string.
   *
   * @return The underlying SQL string
   */
  [[nodiscard]] std::string sql() const;

  [[nodiscard]] utils::result<std::unique_ptr<query_result_impl>, utils::error> fetch_internal() const;

private:
  std::shared_ptr<statement_proxy> statement_proxy_;
  std::unique_ptr<value_writer> bindings_;
  std::shared_ptr<abstract_sql_logger> logger_;
};

template<typename Type>
statement &statement::bind(size_t pos, Type &value) {
  statement_proxy_->bind(pos, value, *bindings_);
  return *this;
}

template<class Type>
statement &statement::bind(const Type &obj) {
  statement_proxy_->bind(obj, *bindings_);
  return *this;
}

template<class Type>
statement &statement::bind(const object_ptr<Type> &ptr) {
  return bind(*ptr);
}

template<class Type>
utils::result<query_result<Type>, utils::error> statement::fetch() {
  std::cout << statement_proxy_->sql() << std::endl;
  statement_proxy_->statement_->query_.result_type = typeid(Type);
  return statement_proxy_->fetch(*bindings_)
  .and_then([this](std::unique_ptr<query_result_impl> &&value) -> utils::result<query_result<Type>, utils::error> {
    auto resolver = statement_proxy_->statement_->query_.resolver->resolver<Type>();
    const auto prototype = value->prototype();
    return utils::ok(query_result<Type>::make_query_result(
      std::forward<decltype(value)>(value),
      resolver,
      [] { return std::make_shared<Type>(); }));
  });
}

template<class Type>
utils::result<object_ptr<Type>, utils::error> statement::fetch_one() {
  std::cout << statement_proxy_->sql() << std::endl;
  statement_proxy_->statement_->query_.result_type = typeid(Type);
  auto result = statement_proxy_->fetch(*bindings_);
  if (!result.is_ok()) {
    return utils::failure(result.err());
  }
  auto resolver = statement_proxy_->statement_->query_.resolver->resolver<Type>();
  const auto prototype = result.value()->prototype();
  auto records = query_result<Type>(
    result.release(),
    resolver,
    [] { return std::make_shared<Type>(); });
  auto first = records.begin();
  if (first == records.end()) {
    return utils::failure(utils::error{error_code::FetchFailed,"Failed to find entity."});
  }

  return utils::ok(first.optr());
}

template<class Type>
utils::result<std::shared_ptr<Type>, utils::error> statement::fetch_one_raw() {
  std::cout << statement_proxy_->sql() << std::endl;
  auto result = statement_proxy_->fetch(*bindings_);
  if (!result.is_ok()) {
    return utils::failure(result.err());
  }

  auto obj = std::make_shared<Type>();
  (*result)->bind(*obj);
  if (!(*result)->fetch(*obj)) {
    return utils::ok(std::shared_ptr<Type>{});
  }
  return utils::ok(obj);
}
}

#endif //QUERY_STATEMENT_HPP
