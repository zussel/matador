#ifndef QUERY_SESSION_HPP
#define QUERY_SESSION_HPP

#include "matador/query/error_code.hpp"
#include "matador/query/internal/delete_query_builder.hpp"
#include "matador/query/internal/select_query_builder.hpp"
#include "matador/query/criteria.hpp"
#include "matador/query/internal/insert_query_builder.hpp"
#include "matador/query/query.hpp"
#include "matador/query/internal/query_contexts.hpp"

#include "matador/query/connection.hpp"
#include "matador/query/connection_pool.hpp"
#include "matador/query/executor.hpp"
#include "matador/query/resolver_service.hpp"
#include "matador/query/statement.hpp"
#include "matador/query/statement_cache.hpp"

#include "matador/query/object_cache.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/basic_schema.hpp"

#include <unordered_map>
#include <utility>

namespace matador::query {

struct session_context {
  session_context(utils::message_bus &bus, std::string dns, const size_t count, const size_t cache_size = 500)
  : bus(bus)
  , dns(std::move(dns))
  , connection_count(count)
  , cache_size(cache_size) {
  }
  utils::message_bus &bus;
  std::string dns;
  size_t connection_count{};
  size_t cache_size{500};
  std::shared_ptr<resolver_service> resolver_service = std::make_shared<resolver_service>();
};

class session final /*: public executor*/ {
public:
  session(session_context &&ctx, const basic_schema &scm);

  /**
   * Insert the given object into the session.
   *
   * @tparam Type Type of object to insert
   * @param obj Object to insert
   * @return Inserted object
   */
  template<typename Type>
  utils::result<object_ptr<Type>, utils::error> insert(object_ptr<Type> obj);
  template<typename Type>
  utils::result<object_ptr<Type>, utils::error> update(const object_ptr<Type> &obj);
  template<typename Type>
  utils::result<void, utils::error> remove(const object_ptr<Type> &obj);

  template<typename Type, typename PrimaryKeyType>
  utils::result<object_ptr<Type>, utils::error> find(const PrimaryKeyType &pk);
  template<typename Type>
  utils::result<query_result<Type>, utils::error> find(query::criteria_ptr clause = {});

  [[nodiscard]] const basic_schema &schema() const;

private:
  connection_pool pool_;
  mutable statement_cache cache_;
  const dialect &dialect_;

  object::object_cache object_cache_;

  const basic_schema &schema_;
  mutable std::unordered_map<std::string, std::vector<column> > prototypes_;
  std::shared_ptr<resolver_service> resolver_service_;
  std::unordered_map<std::type_index, query::query_contexts> contexts_by_type_;
};

template<typename Type>
utils::result<object_ptr<Type>, utils::error> session::insert(object_ptr<Type> obj) {
  if (obj.is_persistent()) {
    return utils::ok(obj);
  }

  if (const auto it = schema_.find(typeid(Type)); it == schema_.end()) {
    return utils::failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  }

  // Build dependency-ordered insert steps (deps first, root last)
  insert_query_builder<Type> iqb(schema_, contexts_by_type_);
  auto steps = iqb.build(obj);
  if (!steps.is_ok()) {
    return utils::failure(make_error(error_code::FailedToBuildQuery, "Failed to build insert dependency queries."));
  }

  // Execute all steps; for Identity steps read RETURNING and write pk back into the object
  for (auto &step : *steps) {
    const auto conn = pool_.acquire();
    if (!conn.valid()) {
      return utils::failure(make_error(error_code::FailedToAcquirePool, "Failed to acquire connection pool for primary key generation."));
    }

    if (const auto result = step->prepare(*conn); !result.is_ok()) {
      return utils::failure(result.err());
    }
    conn.release();

    auto stmt = cache_.acquire(step->ctx());
    if (!stmt.is_ok()) {
      return utils::failure(stmt.err());
    }

    if (const auto result = step->execute(*stmt); !result.is_ok()) {
      return utils::failure(result.err());
    }
  }

  // After successfully executed all inserts, add them to the object cache
  for (auto &step : *steps) {
    if (const auto result = step->finalize(object_cache_, resolver_service_); !result.is_ok()) {
      return utils::failure(result.err());
    }
  }

  return utils::ok(obj);
}

class pk_object_binder final {
public:
  explicit pk_object_binder(statement &stmt, const size_t position)
  : stmt_(stmt)
  , binding_position_(position) {
  }

  template<class Type>
  statement &bind(Type &obj) {
    access::process(*this, obj);
    return stmt_;
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<class Type>
  void on_primary_key(const char * /*id*/, Type &x, const primary_key_options & /*attr*/) {
    stmt_.bind(binding_position_, x);
  }

  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template<class Type>
  static void on_attribute(const char * /*id*/, Type &/*x*/, const column_options& /*attr*/) {}
  template<class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_key_options& /*attr*/) {}
  template<class Pointer>
  static void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {}

  template<class ContainerType>
  static void on_has_many(const char * /*id*/,
                          ContainerType &/*c*/,
                          const char * /*join_column*/,
                          const foreign_key_options &/*attr*/) {}

  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/,
                                  ContainerType &/*c*/,
                                  const char * /*join_column*/,
                                  const char * /*inverse_join_column*/,
                                  const foreign_key_options &/*attr*/) {}

  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/,
                                  ContainerType &/*c*/,
                                  const foreign_key_options &/*attr*/) {}

private:
  statement &stmt_;
  size_t binding_position_{0};
  object_pk_binder pk_binder_{};
};

template<typename Type>
utils::result<object_ptr<Type>, utils::error> session::update(const object_ptr<Type> &obj) {
  const auto it = schema_.find(typeid(Type));
  if (it == schema_.end()) {
    return utils::failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  }
  using namespace matador::utils;
  using namespace matador::query;

  const auto cit = contexts_by_type_.find(it->second.node().info().type_index());
  if (cit == contexts_by_type_.end()) {
    return failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  }
  auto stmt = cache_.acquire(cit->second.update_one);
  if (!stmt.is_ok()) {
    return failure(stmt.err());
  }

  stmt->bind(*obj);
  pk_object_binder binder(stmt.value(), stmt->bind_pos());
  if (const auto update_result = binder.bind(*obj).execute(); !update_result.is_ok()) {
    return utils::failure(update_result.err());
  }
  return utils::ok(object_ptr{obj});
}

template<typename Type>
utils::result<void, utils::error> session::remove(const object_ptr<Type> &obj) {
  if (!obj.is_persistent()) {
    return utils::ok<void>();
  }

  if (const auto it = schema_.find(typeid(Type)); it == schema_.end()) {
    return utils::failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  }

  delete_query_builder<Type> dqb(schema_, contexts_by_type_);
  auto steps = dqb.build(obj);
  if (!steps.is_ok()) {
    return utils::failure(make_error(error_code::FailedToBuildQuery, "Failed to build delete dependency queries."));
  }

  for (auto &step : *steps) {
    const auto conn = pool_.acquire();
    if (!conn.valid()) {
      return utils::failure(make_error(error_code::FailedToAcquirePool, "Failed to acquire connection pool for primary key generation."));
    }

    if (const auto result = step->prepare(*conn); !result.is_ok()) {
      return utils::failure(result.err());
    }
    conn.release();

    auto stmt = cache_.acquire(step->ctx());
    if (!stmt.is_ok()) {
      return utils::failure(stmt.err());
    }

    if (const auto result = step->execute(*stmt); !result.is_ok()) {
      return utils::failure(result.err());
    }
  }

  // After successfully executed all deletes, add them to the object cache
  for (auto &step : *steps) {
    if (const auto result = step->finalize(object_cache_, resolver_service_); !result.is_ok()) {
      return utils::failure(result.err());
    }
  }

  return utils::ok<void>();
  // using namespace matador::utils;
  // using namespace matador::query;
  //
  // const auto col = table_column(it->second.node().info().primary_key_attribute()->name());
  // const auto cit = contexts_by_type_.find(it->second.node().info().type_index());
  // if (cit == contexts_by_type_.end()) {
  //   return failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  // }
  // auto stmt = cache_.acquire(cit->second.delete_one);
  // if (!stmt.is_ok()) {
  //   return failure(stmt.err());
  // }
  //
  // pk_object_binder binder(*stmt, stmt->bind_pos());
  // if (const auto update_result = binder.bind(*obj).execute(); !update_result.is_ok()) {
  //   return utils::failure(update_result.err());
  // }
  // return utils::ok<void>();
}

template<typename Type, typename PrimaryKeyType>
utils::result<object_ptr<Type>, utils::error> session::find(const PrimaryKeyType &pk) {
  const auto it = schema_.find(typeid(Type));
  if (it == schema_.end()) {
    return utils::failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  }

  if (const auto &info = it->second.node().info(); !info.has_primary_key()) {
    return utils::failure(make_error(error_code::FailedToFindPrimaryKey, "Type hasn't primary key."));
  }

  auto resolver = resolver_service_->template resolver<Type>();
  if (!resolver) {
    return utils::failure(utils::error(error_code::UnknownType, "Missing object resolver for inserted type."));
  }

  if (object_cache_.is_loaded<Type>(utils::identifier{pk})) {
    return utils::ok(object_ptr(object_cache_.acquire_proxy<Type>(identifier{pk}, resolver)));
  }

  select_query_builder eqb(schema_);
  auto data = eqb.build<Type>(*it->second.table().primary_key_column() == pk);
  if (!data.is_ok()) {
    return utils::failure(make_error(error_code::FailedToBuildQuery, "Failed to build query for type " + it->second.name() + "."));
  }

  auto ctx = data->compile(dialect_);
  ctx.resolver = resolver_service_;
  auto stmt = cache_.acquire(ctx);
  if (!stmt.is_ok()) {
    return utils::failure(stmt.err());
  }

  return stmt->template fetch_one<Type>();
}

template<typename Type>
utils::result<query_result<Type>, utils::error> session::find(criteria_ptr clause) {
  const auto it = schema_.find(typeid(Type));
  if (it == schema_.end()) {
    return utils::failure(make_error(error_code::UnknownType, "Failed to determine requested type."));
  }

  query::select_query_builder eqb(schema_);
  auto data = eqb.build<Type>(std::move(clause));
  if (!data.is_ok()) {
    return utils::failure(make_error(error_code::FailedToBuildQuery,
                                     "Failed to build query for type " + it->second.name() + "."));
  }

  auto ctx = data->compile(dialect_);
  ctx.resolver = resolver_service_;
  auto stmt = cache_.acquire(ctx);
  if (!stmt.is_ok()) {
    return utils::failure(stmt.err());
  }

  return stmt->template fetch<Type>();
}
}
#endif //QUERY_SESSION_HPP
