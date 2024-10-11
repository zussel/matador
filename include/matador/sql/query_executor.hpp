#ifndef QUERY_EXECUTOR_HPP
#define QUERY_EXECUTOR_HPP

#include "matador/sql/sql_error.hpp"

#include "matador/utils/result.hpp"

#include <memory>

namespace matador::sql {

class statement;
class query_result_impl;

class query_executor
{
public:
  virtual ~query_executor() = default;

  [[nodiscard]] virtual utils::result<size_t, sql_error> execute(const query_compile_context &ctx) const = 0;
  [[nodiscard]] virtual utils::result<std::unique_ptr<query_result_impl>, sql_error> fetch(const query_compile_context &ctx) const = 0;
  [[nodiscard]] virtual statement prepare(const query_compile_context &ctx) const = 0;
  [[nodiscard]] virtual std::string str(const query_compile_context &ctx) const = 0;
};

}

#endif //QUERY_EXECUTOR_HPP
