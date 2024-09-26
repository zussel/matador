#ifndef QUERY_EXECUTOR_HPP
#define QUERY_EXECUTOR_HPP

#include <memory>

namespace matador::sql {

class statement;
class query_result_impl;

class query_executor
{
public:
  virtual ~query_executor() = default;

  [[nodiscard]] virtual size_t execute(const query_compile_context &ctx) const = 0;
  // [[nodiscard]] virtual query_result<record> fetch(const query_compile_context &ctx) const = 0;
  [[nodiscard]] virtual std::unique_ptr<query_result_impl> fetch(const query_compile_context &ctx) const = 0;
  [[nodiscard]] virtual statement prepare(const query_compile_context &ctx) const = 0;
  [[nodiscard]] virtual std::string str(const query_compile_context &ctx) const = 0;
};

}

#endif //QUERY_EXECUTOR_HPP
