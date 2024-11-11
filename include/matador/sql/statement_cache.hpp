#ifndef QUERY_STATEMENT_CACHE_HPP
#define QUERY_STATEMENT_CACHE_HPP

#include "matador/sql/statement.hpp"
#include "matador/sql/connection_pool.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace matador::sql {

class connection;

struct cache_info
{
  statement statement_;
//  std::unique_ptr<statement> statement_;
  size_t connection_id_;
};

class statement_cache
{
public:
  statement_cache(connection_pool<connection> &pool, const dialect &d);

  statement& prepare(query_compile_context &&context);
  void release(const statement &stmt);

private:
  mutable std::mutex mutex_;
  size_t max_cache_size_{256};
  std::hash<std::string> hash_;
  using statement_map = std::unordered_map<size_t, cache_info>;
  statement_map statement_map_;
  connection_pool<connection> &pool_;
  const dialect &dialect_;
};

}
#endif //QUERY_STATEMENT_CACHE_HPP
