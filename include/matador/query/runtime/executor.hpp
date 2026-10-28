#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "matador/query/resolver_service.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>

namespace matador::query {
struct query_context;
struct execute_result;
class query_result_impl;
class statement;

class executor {
public:
  virtual ~executor() = default;
  [[nodiscard]] virtual const class dialect& dialect() const = 0;
  [[nodiscard]] virtual result<execute_result, error> execute(const query_context &ctx) const = 0;
  [[nodiscard]] virtual result<std::unique_ptr<query_result_impl>, error> fetch(const query_context &ctx) const = 0;
  [[nodiscard]] virtual result<statement, error> prepare(const query_context &ctx) const = 0;
  [[nodiscard]] virtual std::string str(const query_context &ctx) const = 0;
  [[nodiscard]] virtual std::shared_ptr<resolver_service> resolver() const = 0;
};

}

#endif //EXECUTOR_HPP
