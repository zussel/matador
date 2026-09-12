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
  template <class Type>
  result<query_result<Type>, error> fetch_all(executor &exec) {
    auto result = fetch(exec, typeid(Type));
    if (!result.is_ok()) {
      return failure<error>(result.err());
    }

    const auto prototype = result.value()->prototype();
    auto resolver = exec.resolver()->resolver<Type>();
    return query_result<Type>::make_query_result(result, resolver);
  }
  [[nodiscard]] result<query_result<record>, error>
  fetch_all(const executor &exec) const;

  template <class Type>
  result<object_ptr<Type>, error> fetch_one(executor &exec) {
    auto result = fetch(exec, typeid(Type));
    if (!result.is_ok()) {
      return failure<error>(result.err());
    }

    const auto prototype = result.value()->prototype();
    auto resolver = exec.resolver()->resolver<Type>();
    return query_result<Type>::make_query_result(result, resolver, prototype).and_then([](query_result<Type> &qr) {
      auto first = qr.begin();
      if (first == qr.end()) {
        return ok<query_result<Type>>(object_ptr<Type>{});
      }

      return ok<query_result<Type>>(first.optr());
    });
  }

  [[nodiscard]] result<std::optional<record>, error>
  fetch_one(const executor &exec) const;

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

  [[nodiscard]] std::string str(const dialect &d) const;
  [[nodiscard]] query_context compile(const dialect &d) const;

private:
  [[nodiscard]] result<std::unique_ptr<query_result_impl>, error>
  fetch(const executor &exec, const std::type_index &index) const;
};

} // namespace matador::query

#endif // FETCHABLE_QUERY_HPP
