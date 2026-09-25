#ifndef MATADOR_INSERT_STEP_HPP
#define MATADOR_INSERT_STEP_HPP

#include "matador/query/abstract_pk_generator.hpp"
#include "matador/query/execute_step.hpp"
#include "matador/query/error_code.hpp"

#include "matador/object/object_cache.hpp"
#include "matador/object/object_ptr.hpp"

#include "matador/sql/execute_result.hpp"
#include "matador/sql/resolver_service.hpp"
#include "matador/sql/statement.hpp"

namespace matador::query {
template<typename ObjectType>
utils::result<void, utils::error> finalize_inserted_object(object::object_ptr<ObjectType> &ptr,
                                                           const utils::identifier& pk,
                                                           object::object_cache &cache,
                                                           const std::shared_ptr<sql::resolver_service> &resolver_service) {
  if (!ptr) {
    return utils::failure(utils::error(error_code::InvalidObject, "Inserted object is null."));
  }

  if (!resolver_service) {
    return utils::failure(utils::error(error_code::UnknownType, "Missing resolver service."));
  }

  auto resolver = resolver_service->template object_resolver<ObjectType>();
  if (!resolver) {
    return utils::failure(utils::error(error_code::UnknownType, "Missing object resolver for inserted type."));
  }

  if (!cache.import<ObjectType>(pk, ptr.proxy(), resolver)) {
    return utils::failure(utils::error(error_code::FailedToFindObject, "Object cache already contains another live proxy for inserted object."));
  }

  ptr.change_state(object::object_state::Persistent);
  return utils::ok<void>();
}

template <typename ObjectType>
class insert_step_pk_generated : public execute_step {
public:
  insert_step_pk_generated(sql::query_context ctx, const object::object_ptr<ObjectType>& ptr, abstract_pk_generator& pk_generator)
  : execute_step(std::move(ctx))
  , ptr_(ptr)
  , pk_generator_(pk_generator){}

  utils::result<void, utils::error> prepare(sql::executor& conn) override {
    auto result = pk_generator_.next_id(conn);
    if (!result.is_ok()) {
      return utils::failure(result.err());
    }
    id_ = *result;
    pk_accessor_.set(*ptr_, id_);

    return utils::ok<void>();
  }

  utils::result<void, utils::error> execute(sql::statement& stmt) override {
    stmt.bind(*ptr_);

    if (const auto exec_result = stmt.execute(); !exec_result.is_ok()) {
      return utils::failure(exec_result.err());
    }

    ptr_.change_state(object::object_state::Persistent);
    return utils::ok<void>();
  }

  utils::result<void, utils::error> finalize(object::object_cache& cache, const resolver_service_ptr& resolver_service) override {
    return finalize_inserted_object(ptr_, id_, cache, resolver_service);
  }

private:
  object::object_ptr<ObjectType> ptr_;
  abstract_pk_generator& pk_generator_;
};

template <typename ObjectType>
class insert_step_pk_identity : public execute_step {
public:
  insert_step_pk_identity(sql::query_context ctx, const object::object_ptr<ObjectType>& ptr, std::string pk_column_name)
  : execute_step(std::move(ctx))
  , ptr_(ptr)
  , pk_column_name_(std::move(pk_column_name)){}

  utils::result<void, utils::error> prepare(sql::executor&) override {
    return utils::ok<void>();
  }

  utils::result<void, utils::error> execute(sql::statement& stmt) override {
    stmt.bind(*ptr_);

    auto result = stmt.fetch_one();
    if (!result.is_ok()) {
      return utils::failure(result.err());
    }
    if (!result.value().has_value()) {
      return utils::failure(utils::error(error_code::FailedToFindObject, "Failed to insert object and retrieve identity."));
    }

    auto rec = result->value();
    const auto& f = rec.at(pk_column_name_);
    if (auto res = id_.assign(f.value()); !res.is_ok()) {
      return utils::failure(res.err());
    }
    pk_accessor_.set(*ptr_, id_);

    ptr_.change_state(object::object_state::Persistent);
    return utils::ok<void>();
  }

  utils::result<void, utils::error> finalize(object::object_cache& cache, const resolver_service_ptr& resolver_service) override {
    return finalize_inserted_object(ptr_, id_, cache, resolver_service);
  }

private:
  object::object_ptr<ObjectType> ptr_;
  std::string pk_column_name_;
};

template <typename ObjectType>
class insert_step_pk_manual : public execute_step {
public:
  insert_step_pk_manual(sql::query_context ctx, const object::object_ptr<ObjectType>& ptr)
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

    id_ = object::primary_key_resolver::resolve_object(*ptr_).pk;
    ptr_.change_state(object::object_state::Persistent);
    return utils::ok<void>();
  }

  utils::result<void, utils::error> finalize(object::object_cache& cache, const resolver_service_ptr& resolver_service) override {
    return finalize_inserted_object(ptr_, id_, cache, resolver_service);
  }

private:
  object::object_ptr<ObjectType> ptr_;
};

template <typename ObjectType>
class insert_step_relation : public execute_step {
public:
  insert_step_relation(sql::query_context ctx, const object::object_ptr<ObjectType>& ptr)
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
#endif //MATADOR_INSERT_STEP_HPP
