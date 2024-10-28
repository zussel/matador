#ifndef QUERY_STATEMENT_HPP
#define QUERY_STATEMENT_HPP

#include "matador/sql/basic_sql_logger.hpp"
#include "matador/sql/query_result.hpp"
#include "matador/sql/statement_impl.hpp"
#include "matador/sql/sql_error.hpp"

#include <memory>

namespace matador::sql {
namespace detail {
template<class Type>
class identifier_binder;
}

class statement {
public:
  /**
   * Creates a statement initialized from the
   * given statement implementation object holding
   * the implementation for the selected database
   *
   * @param impl The statement implementation object
   * @param logger The logger handler to write sql log messages to
   */
  explicit statement(std::unique_ptr<statement_impl> impl,
                     const std::shared_ptr<basic_sql_logger> &logger = std::make_shared<null_sql_logger>());
  /**
   * Copy move constructor for statement
   *
   * @param x The statement to move from
   */
  statement(statement &&x) noexcept
    : statement_(std::move(x.statement_))
      , logger_(std::move(x.logger_)) {
  }

  /**
   * Assignment move constructor for statement
   *
   * @param x The statement to move from
   * @return Reference to this
   */
  statement &operator=(statement &&x) noexcept {
    statement_ = std::move(x.statement_);
    logger_ = std::move(x.logger_);
    return *this;
  }

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
  template<typename Type>
  statement &bind(size_t pos, Type &value);

  /**
   * Executes the prepared statement and returns
   * the number of affected rows.
   *
   * @return The number of affected rows
   */
  [[nodiscard]] utils::result<size_t, sql_error> execute() const;

  /**
   * Fetches the result of the prepared
   * statement. If prepared statement was not
   * a SELECT statement an empty query result set
   * is returned.
   *
   * @tparam Type Type of the fetched result
   * @return The query result set
   */
  template<class Type>
  utils::result<query_result<Type>, sql_error> fetch();
  /**
   * Fetches the result of the prepared
   * statement. The type is record representing an
   * unknown variable type.
   * If prepared statement was not
   * a SELECT statement an empty query result set
   * is returned.
   *
   * @return The query result set
   */
  [[nodiscard]] utils::result<query_result<record>, sql_error> fetch() const;
  /**
   * Fetches the first result of a prepared statement.
   * If prepared statement is empty or not
   * a SELECT statement a nullptr is returned.
   *
   * @tparam Type Type of the fetched result
   * @return The query result set
   */
  template<class Type>
  utils::result<std::unique_ptr<Type>, sql_error> fetch_one();
  /**
   * Fetches the first result of a prepared statement.
   * The type is record representing an unknown variable type.
   * If prepared statement is empty or not
   * a SELECT statement a nullptr is returned.
   *
   * @return The query result set
   */
  [[nodiscard]] utils::result<std::optional<record>, sql_error> fetch_one() const;

  /**
   * Resets the prepared statement to
   * reuse it.
   */
  void reset() const;

private:
  template<class Type>
  friend class detail::identifier_binder;

private:
  std::unique_ptr<statement_impl> statement_;
  std::shared_ptr<basic_sql_logger> logger_;
};

template<typename Type>
statement &statement::bind(size_t pos, Type &value) {
  statement_->bind(pos, value);
  return *this;
}

template<class Type>
statement &statement::bind(const Type &obj) {
  statement_->bind_object(obj);
  return *this;
}

template<class Type>
utils::result<query_result<Type>, sql_error> statement::fetch() {
  return statement_->fetch().and_then([](std::unique_ptr<query_result_impl> &&value) {
    return utils::ok(query_result<Type>(std::forward<decltype(value)>(value)));
  });
//  if (!result.is_ok()) {
//    return utils::error(result.err());
//  }
//  return query_result<Type>(result.release());
}

template<class Type>
utils::result<std::unique_ptr<Type>, sql_error> statement::fetch_one() {
  auto result = statement_->fetch();
  if (!result.is_ok()) {
    return utils::error(result.err());
  }
  auto records = query_result<Type>(result.release());
  auto first = records.begin();
  if (first == records.end()) {
    return utils::ok(std::unique_ptr<Type>{nullptr});
  }

  return utils::ok(std::unique_ptr<Type>{first.release()});
}
}

#endif //QUERY_STATEMENT_HPP
