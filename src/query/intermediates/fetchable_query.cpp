#include "matador/query/intermediates/fetchable_query.hpp"

#include "matador/query/query_builder.hpp"

#include "matador/query/executor.hpp"
#include "matador/query/field.hpp"
// #include "matador/query/statement.hpp"

namespace matador::query {
utils::result<query_result<record>, error> fetchable_query::fetch_all(const executor &exec) const {
  query_builder compiler;
  auto ctx = compiler.build(*context_, exec.dialect(), std::nullopt);
  ctx.resolver = exec.resolver();
  return exec.fetch(ctx)
    .and_then([](auto &&res) -> result<query_result<record>, error> {
      auto prototype = res->prototype();
      return query_result<record>::make_query_result(std::forward<decltype(res)>(res), std::move(prototype));
    });
}

result<std::optional<record>, error> fetchable_query::fetch_one(const executor &exec) const {
  query_builder compiler;
  auto ctx = compiler.build(*context_, exec.dialect(), std::nullopt);
  ctx.resolver = exec.resolver();
  auto fetch_result = exec.fetch(ctx);
  if (!fetch_result.is_ok()) {
    return failure<error>(fetch_result.err());
  }

  const auto prototype = fetch_result.value()->prototype();
  auto records_result = query_result<record>::make_query_result(std::move(*fetch_result), prototype);
  if (!records_result.is_ok()) {
    return failure<error>(records_result.err());
  }

  auto records = records_result.release();
  auto first = records.begin();
  if (first == records.end()) {
    return ok<std::optional<record>>(std::optional<record>{std::nullopt});
  }

  return ok<std::optional<record>>(std::optional{std::move(*first)});
}

std::string fetchable_query::str(const dialect &d) const {
  return compile(d).sql;
}

query_context fetchable_query::compile(const dialect &d) const {
  query_builder compiler;
  return compiler.build(*context_, d, std::nullopt);
}

result<std::unique_ptr<query_result_impl>, error> fetchable_query::fetch(const executor &exec, const std::type_index& index) const {
  auto ctx = compile(exec.dialect());
  ctx.resolver = exec.resolver();
  ctx.result_type = index;
  return exec.fetch(ctx);
}

// result<statement, error> fetchable_query::prepare(executor &exec) const {
  // auto ctx = compile(exec.dialect());
  // ctx.resolver = exec.resolver();
  // return exec.prepare(ctx);
// }

}