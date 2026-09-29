#ifndef MATADOR_EXECUTE_STEP_HPP
#define MATADOR_EXECUTE_STEP_HPP

#include "matador/query/abstract_pk_generator.hpp"
#include "matador/query/executor.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/internal/primary_key_accessor.hpp"
#include "matador/query/object_cache.hpp"
#include "matador/query/query_context.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

namespace matador::query {
using resolver_service_ptr = std::shared_ptr<resolver_service>;

class execute_step {
public:
  explicit execute_step(query_context ctx)
  : ctx_(std::move(ctx)) {}
  virtual ~execute_step() = default;

  virtual utils::result<void, utils::error> prepare(executor &conn) = 0;
  virtual utils::result<void, utils::error> execute(statement &stmt) = 0;
  virtual utils::result<void, utils::error> finalize(object_cache& cache, const resolver_service_ptr& resolver_service) = 0;

  [[nodiscard]] const query_context& ctx() const { return ctx_; }

protected:
  identifier id_;
  primary_key_accessor pk_accessor_;
  query_context ctx_;
};
}
#endif //MATADOR_EXECUTE_STEP_HPP
