#ifndef FETCHABLE_QUERY_HPP
#define FETCHABLE_QUERY_HPP

#include "matador/query/intermediates/intermediate.hpp"

#include "matador/query/executor.hpp"
#include "matador/query/query_result.hpp"
#include "matador/query/record.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

namespace matador::query {
class executor;
class statement;
class fetchable_query : public intermediate {
protected:
  using intermediate::intermediate;

public:
  /**
   * Fetches every row and resolves it as an object of Type.
   *
   * @param exec Executor used to run the compiled query.
   * @return Resolved rows or the database error returned by the executor.
   */
  template <class Type>
  result<query_result<Type>, error> fetch_all(executor &exec) {
    auto result = fetch(exec, typeid(Type));
    if (!result.is_ok()) {
      return failure<error>(result.release_error());
    }

    auto resolver = exec.resolver()->resolver<Type>();
    return query_result<Type>::make_query_result(
      result.release(),
      std::move(resolver),
      [] { return std::make_shared<Type>(); }
    );
  }
  /**
   * Fetches every row as a generic record.
   *
   * @param exec Executor used to run the compiled query.
   * @return Records or the database error returned by the executor.
   */
  [[nodiscard]] result<query_result<record>, error>
  fetch_all(const executor &exec) const;

  /**
   * Fetches the first row and resolves it as an object of Type.
   *
   * @param exec Executor used to run the compiled query.
   * @return An empty object pointer when no row matches, or a database error.
   */
  template <class Type>
  result<object_ptr<Type>, error> fetch_one(executor &exec) {
    auto result = fetch(exec, typeid(Type));
    if (!result.is_ok()) {
      return failure<error>(result.release_error());
    }

    auto resolver = exec.resolver()->resolver<Type>();
    return query_result<Type>::make_query_result(
      result.release(),
      std::move(resolver),
      [] { return std::make_shared<Type>(); }
    ).and_then([](query_result<Type> &&qr) -> matador::result<object_ptr<Type>, error> {
      auto first = qr.begin();
      if (first == qr.end()) {
        return ok<object_ptr<Type>>(object_ptr<Type>{});
      }

      return ok<object_ptr<Type>>(*first);
    });
  }

  /**
   * Fetches the first row as a generic record.
   *
   * @param exec Executor used to run the compiled query.
   * @return An empty optional when no row matches, or a database error.
   */
  [[nodiscard]] result<std::optional<record>, error>
  fetch_one(const executor &exec) const;

  /**
   * Fetches the first column of the first row as Type.
   *
   * @param exec Executor used to run the compiled query.
   * @return An empty optional when no row matches, or a conversion/database error.
   */
  template <typename Type>
  result<std::optional<Type>, error> fetch_value(const executor &exec) {
    auto fetch_result = fetch_one(exec);
    if (!fetch_result.is_ok()) {
      return failure<error>(fetch_result.err());
    }

    const auto &record_result = fetch_result.value();
    if (record_result.has_value()) {
      return ok<std::optional<Type>>(std::optional<Type>{record_result.value().at(0).as<Type>().value()});
    }
    return ok<std::optional<Type>>(std::optional<Type>{std::nullopt});
  }

  // [[nodiscard]] result<statement, error> prepare(executor &exec) const;

  /**
   * Compiles this query to SQL for a dialect.
   *
   * @param d Dialect used to serialize the query.
   * @return The generated SQL string.
   */
  [[nodiscard]] std::string str(const dialect &d) const;
  /**
   * Compiles this query to an executable query context.
   *
   * @param d Dialect used to serialize the query.
   * @return SQL, command type, result prototype, and bind-variable metadata.
   */
  [[nodiscard]] query_context compile(const dialect &d) const;

private:
  [[nodiscard]] result<std::unique_ptr<query_result_impl>, error>
  fetch(const executor &exec, const std::type_index &index) const;
};

} // namespace matador::query

#endif // FETCHABLE_QUERY_HPP
