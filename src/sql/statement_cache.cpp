#include "matador/sql/connection.hpp"
#include "matador/sql/statement_cache.hpp"

namespace matador::sql {

statement_cache::statement_cache(connection_pool<connection>& pool, const dialect &d)
: pool_(pool)
, dialect_(d)
{}

statement &statement_cache::prepare(query_compile_context &&context) {
  std::lock_guard<std::mutex> guard(mutex_);
  const connection_impl* impl{};
  const auto ctx = dialect_.compile(context,  *impl);
  const auto key = hash_(ctx.sql);
  const auto it = statement_map_.find(key);
  if (it == statement_map_.end()) {
    // prepared statement not found
    // create a new one
      auto conn = pool_.acquire();

      auto stmt = conn->prepare(ctx);
    // try to get connection by id
    // cache_info info {conn.prepare(std::move(context)), 1};
//    it = statement_map_.emplace(key, {conn.prepare(std::move(context))});
  }
  return it->second.statement_;
}

void statement_cache::release(const statement &/*stmt*/) {

}
}