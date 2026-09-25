#ifndef MATADOR_DELETE_STEP_HPP
#define MATADOR_DELETE_STEP_HPP

#include "matador/query/error_code.hpp"
#include "matador/query/execute_step.hpp"
#include "matador/query/internal/identifier_statement_binder.hpp"
#include "matador/query/statement.hpp"
#include "matador/query/object_ptr.hpp"

namespace matador::query {
template<typename ObjectType>
class delete_step_object final : public execute_step {
public:
  delete_step_object(sql::query_context ctx, const object::object_ptr<ObjectType> &ptr)
  : execute_step(std::move(ctx))
  , ptr_(ptr) {}

  utils::result<void, utils::error> prepare(sql::executor &/*conn*/) override {
    id_ = ptr_.primary_key();
    return utils::ok<void>();
  }

  utils::result<void, utils::error> execute(sql::statement &stmt) override {
    if (!ptr_) {
      return utils::failure(utils::error{error_code::InvalidObject, "Object is null"});
    }

    sql::identifier_statement_binder binder(stmt, 0);
    binder.bind(id_);

    if (const auto result = stmt.execute(); !result.is_ok()) {
      return utils::failure(result.err());
    }

    return utils::ok<void>();
  }

  utils::result<void, utils::error> finalize(object::object_cache &cache, const resolver_service_ptr& /*resolver_service*/) override {
    if (!ptr_) {
      return utils::failure(utils::error{error_code::InvalidObject, "Object is null"});
    }

    cache.erase<ObjectType>(id_);
    ptr_.change_state(object::object_state::Transient);

    return utils::ok<void>();
  }

private:
  object::object_ptr<ObjectType> ptr_;
};

template <typename ObjectType>
class delete_step_relation : public execute_step {
public:
  delete_step_relation(sql::query_context ctx, const object::object_ptr<ObjectType>& ptr)
  : execute_step(std::move(ctx))
  , ptr_(ptr) {}

  utils::result<void, utils::error> prepare(sql::executor &) override {
    return utils::ok<void>();
  }
  utils::result<void, utils::error> execute(sql::statement &stmt) override {
    stmt.bind(*ptr_);
    if (const auto exec_result = stmt.execute(); !exec_result.is_ok()) {
      return utils::failure(exec_result.err());
    }

    return utils::ok<void>();
  }

  utils::result<void, utils::error> finalize(object::object_cache& /*cache*/, const resolver_service_ptr& /*resolver_service*/) override {
    return utils::ok<void>();
  }

private:
  object::object_ptr<ObjectType> ptr_;
};

}

#endif //MATADOR_DELETE_STEP_HPP
