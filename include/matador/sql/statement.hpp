#ifndef QUERY_STATEMENT_HPP
#define QUERY_STATEMENT_HPP

#include "matador/sql/basic_sql_logger.hpp"
#include "matador/sql/object_parameter_binder.hpp"
#include "matador/sql/query_result.hpp"
#include "matador/sql/statement_impl.hpp"

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
      , object_binder_(std::move(x.object_binder_))
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
    object_binder_ = std::move(x.object_binder_);
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
  size_t execute();

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
  query_result<Type> fetch();
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
  query_result<record> fetch();
  template<class Type>
  /**
   * Fetches the first result of a prepared statement.
   * If prepared statement is empty or not
   * a SELECT statement an nullptr is returned.
   *
   * @tparam Type Type of the fetched result
   * @return The query result set
   */
  std::unique_ptr<Type> fetch_one();
  /**
   * Fetches the first result of a prepared statement.
   * The type is record representing an unknown variable type.
   * If prepared statement is empty or not
   * a SELECT statement an nullptr is returned.
   *
   * @return The query result set
   */
  std::optional<record> fetch_one();

  /**
   * Resets the prepared statement to
   * reuse it.
   */
  void reset();

private:
  template<class Type>
  friend class detail::identifier_binder;

private:
  std::unique_ptr<statement_impl> statement_;
  object_parameter_binder object_binder_;
  std::shared_ptr<basic_sql_logger> logger_;
};

template<typename Type>
statement &statement::bind(size_t pos, Type &value) {
  statement_->bind(pos, value);
  return *this;
}

template<class Type>
statement &statement::bind(const Type &obj) {
  object_binder_.reset();
  matador::access::process(object_binder_, obj);
  return *this;
}

template<class Type>
query_result<Type> statement::fetch() {
  return query_result<Type>(statement_->fetch());
}

template<class Type>
std::unique_ptr<Type> statement::fetch_one() {
  auto result = query_result<Type>(statement_->fetch());
  auto first = result.begin();
  if (first == result.end()) {
    return nullptr;
  }

  return std::unique_ptr<Type>{first.release()};
}
}

#endif //QUERY_STATEMENT_HPP
