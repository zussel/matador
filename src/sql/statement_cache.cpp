#include "matador/sql/connection.hpp"
#include "matador/sql/statement_cache.hpp"

namespace matador::sql {
statement &statement_cache::acquire(query_context &&context, const connection &conn) {
  std::lock_guard<std::mutex> guard(mutex_);
  auto key = hash_(context.sql);
  auto it = statement_map_.find(key);
  if (it == statement_map_.end()) {
    // cache_info info {conn.prepare(std::move(context)), 1};
//    it = statement_map_.emplace(key, {conn.prepare(std::move(context))});
  }
  return it->second.statement_;
}

void statement_cache::release(const statement &/*stmt*/) {

}
}