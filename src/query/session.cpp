#include "matador/query/session.hpp"

#include "matador/query/backend_provider.hpp"
#include "matador/query/dialect.hpp"

#include "matador/query/basic_schema.hpp"
#include "matador/query/schema_utils.hpp"

#include <stdexcept>

namespace matador::query {
utils::error make_error(const error_code ec, const std::string &msg) {
  return utils::error(ec, msg);
}

session::session(session_context&& ctx, const basic_schema &scm)
: pool_(ctx.dns, ctx.connection_count, [ctx](const connection_info& info) { return connection(info, ctx.resolver_service); })
, cache_(ctx.bus, pool_, ctx.cache_size)
, dialect_(backend_provider::instance().connection_dialect(pool_.info().type))
, object_cache_(ctx.bus)
, schema_(scm)
, resolver_service_(ctx.resolver_service) {
  using namespace matador::utils;
  for (const auto &[type, node] : schema_) {
    query_contexts queries = to_query_contexts(node, dialect_);

    queries.insert.resolver = resolver_service_;
    queries.update_one.resolver = resolver_service_;
    queries.delete_one.resolver = resolver_service_;
    queries.select_one.resolver = resolver_service_;
    queries.select_all.resolver = resolver_service_;
    contexts_by_type_[node.node().type_index()] = queries;
  }

  for (const auto &pair : schema_.resolver_producers()) {
    auto res = pair.second->build_query(dialect_).and_then([this](query_context&& query_ctx) -> result<statement, error> {
      query_ctx.resolver = resolver_service_;
      return cache_.acquire(query_ctx);
    }).and_then([&pair, this](statement&& stmt) -> result<void, error> {
      resolver_service_->register_object_resolver(pair.second->produce(std::move(stmt)));

      return ok<void>();
    }).or_else([](const auto &err) {
      return failure(err);
    });

    if (!res) {
      throw std::runtime_error(res.err().message());
    }
  }

    for (const auto &pair : schema_.joined_object_resolver_producers()) {
    auto res = pair.second->build_query(dialect_).and_then([this](query_context&& query_ctx) -> result<statement, error> {
      query_ctx.resolver = resolver_service_;
      return cache_.acquire(query_ctx);
    }).and_then([&pair, this](statement&& stmt) -> result<void, error> {
      resolver_service_->register_joined_object_resolver(pair.second->produce(std::move(stmt)), pair.second->root_type(), pair.second->collection_name());

      return ok<void>();
    }).or_else([](const auto &err) {
      return failure(err);
    });

    if (!res) {
      throw std::runtime_error(res.err().message());
    }
  }

  for (const auto &pair : schema_.collection_resolver_producers()) {
    auto res = pair.second->build_query(dialect_).and_then([this](query_context&& query_ctx) -> result<statement, error> {
      query_ctx.resolver = resolver_service_;
      return cache_.acquire(query_ctx);
    }).and_then([&pair, this](statement&& stmt) -> result<void, error> {
      resolver_service_->register_collection_resolver(pair.second->produce(std::move(stmt), *resolver_service_));

      return ok<void>();
    }).or_else([](const auto &err) {
      return failure(err);
    });

    if (!res) {
      throw std::runtime_error(res.err().message());
    }
  }
}

const basic_schema & session::schema() const {
  return schema_;
}
}
