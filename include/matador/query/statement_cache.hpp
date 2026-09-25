#ifndef STATEMENT_CACHE_HPP
#define STATEMENT_CACHE_HPP

#include "matador/query/executor.hpp"
#include "matador/query/statement.hpp"

#include "matador/utils/message_bus.hpp"

#include <list>
#include <mutex>
#include <unordered_map>

namespace matador::query {

class connection_pool;

struct statement_event {
  std::string sql{};
  std::chrono::steady_clock::time_point timestamp{};
};

struct statement_accessed_event : statement_event {};
struct statement_added_event : statement_event {};
struct statement_evicted_event : statement_event {};

struct statement_lock_failed_event : statement_event {
  std::chrono::nanoseconds duration{};
};

struct statement_lock_acquired_event : statement_event {
  std::chrono::nanoseconds duration{};
};

struct statement_execution_event : statement_event {
  std::chrono::nanoseconds duration{};
};

struct statement_cache_config {
  size_t max_size;
};

class statement_cache final {
private:
  using list_iterator = std::list<size_t>::iterator;

  struct cache_entry {
    statement stmt;
    std::chrono::steady_clock::time_point last_access;
    list_iterator position;
  };

public:
  statement_cache(utils::message_bus &bus, connection_pool &pool, size_t max_size = 50);
  statement_cache(const statement_cache &) = delete;
  statement_cache &operator=(const statement_cache &) = delete;
  statement_cache(statement_cache &&) = delete;
  statement_cache &operator=(statement_cache &&) = delete;
  ~statement_cache() = default;

  [[nodiscard]] utils::result<statement, utils::error> acquire(const query_context &ctx);

  [[nodiscard]] size_t size() const;
  [[nodiscard]] size_t capacity() const;
  [[nodiscard]] bool empty() const;

  template<class EventType>
  utils::subscription subscribe(std::function<void(const EventType&)> handler) {
    return bus_.subscribe<EventType>(handler);
  }

  [[nodiscard]] connection_pool& pool() const;

private:
  size_t max_size_{};
  std::list<size_t> usage_list_;  // LRU: front = most recent, back = least recent
  std::unordered_map<size_t, cache_entry> cache_map_;
  std::mutex mutex_;

  connection_pool &pool_;
  const dialect &dialect_;

  utils::message_bus &bus_;
};

}
#endif //STATEMENT_CACHE_HPP
