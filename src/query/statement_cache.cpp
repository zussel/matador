#include "matador/query/statement_cache.hpp"
#include "matador/query/backend_provider.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/connection_pool.hpp"

#include <atomic>
#include <thread>

namespace matador::query {
namespace {
class statement_cache_proxy final : public statement_proxy {
private:
  struct execution_metrics {
    std::chrono::steady_clock::time_point lock_attempt_start;
    std::chrono::steady_clock::time_point lock_acquired{};
    std::chrono::steady_clock::time_point execution_start{};
    std::chrono::steady_clock::time_point execution_end{};
    size_t lock_attempts{0};
  };

public:
  struct retry_config {
    size_t max_attempts{10};
    std::chrono::milliseconds initial_wait{10};
    std::chrono::milliseconds max_wait{250};
  };

  statement_cache_proxy(utils::message_bus &bus, std::unique_ptr<statement_impl> &&stmt,
                        connection_pool &pool, const size_t connection_id)
      : statement_proxy(std::move(stmt)), pool_(pool), connection_id_(connection_id), bus_(bus) {}

  utils::result<execute_result, utils::error> execute(value_writer &bindings) override {
    execution_metrics metrics{std::chrono::steady_clock::now()};

    auto result = try_with_retry(
        [this, &bindings, &metrics]() -> utils::result<execute_result, utils::error> {
          const auto query = sql();
          if (!try_lock()) {
            ++metrics.lock_attempts;
            const auto now = std::chrono::steady_clock::now();
            bus_.publish(statement_lock_failed_event{query, now, now - metrics.lock_attempt_start});
            return utils::failure(
                utils::error{error_code::StatementLocked,
                             "Failed to execute statement because it is already in use"});
          }
          const auto now = std::chrono::steady_clock::now();
          bus_.publish(statement_lock_acquired_event{query, now, now - metrics.lock_attempt_start});

          auto guard = statement_guard(*this);
          if (const auto conn = pool_.acquire(connection_id_); !conn.valid()) {
            return utils::failure(
                utils::error{error_code::ExecuteFailed,
                             "Failed to execute statement because couldn't lock connection"});
          }

          metrics.execution_start = std::chrono::steady_clock::now();
          auto execution_result = statement_->execute(bindings);
          metrics.execution_end = std::chrono::steady_clock::now();

          bus_.publish(statement_execution_event{query, metrics.execution_end,
                                                 metrics.execution_end - metrics.execution_start});

          return execution_result;
        });

    return result;
  }

  utils::result<std::unique_ptr<query_result_impl>, utils::error>
  fetch(value_writer &bindings) override {
    execution_metrics metrics{std::chrono::steady_clock::now()};

    auto result = try_with_retry(
        [this, &bindings,
         &metrics]() -> utils::result<std::unique_ptr<query_result_impl>, utils::error> {
          if (!try_lock()) {
            ++metrics.lock_attempts;
            return utils::failure(
                utils::error{error_code::StatementLocked,
                             "Failed to execute statement because it is already in use"});
          }
          auto guard = statement_guard(*this);
          if (const auto conn = pool_.acquire(connection_id_); !conn.valid()) {
            return utils::failure(
                utils::error{error_code::ExecuteFailed,
                             "Failed to execute statement because couldn't lock connection"});
          }
          return statement_->fetch(bindings);
        });

    return result;
  }

protected:
  [[nodiscard]] bool try_lock() {
    bool expected = false;
    return locked_.compare_exchange_strong(expected, true);
  }

  template <typename Func> [[nodiscard]] auto try_with_retry(Func &&func) -> decltype(func()) {
    auto current_wait = config_.initial_wait;

    for (size_t attempt = 0; attempt < config_.max_attempts; ++attempt) {
      if (auto result = func();
          result.is_ok() || result.err().ec() != error_code::StatementLocked) {
        return result;
      }

      if (attempt + 1 < config_.max_attempts) {
        std::this_thread::sleep_for(current_wait);
        current_wait = (std::min)(current_wait * 2, config_.max_wait);
      }
    }

    return utils::failure(utils::error{error_code::StatementLocked,
                                       "Failed to execute statement because it is already in use"});
  }

  void unlock() {
    locked_.store(false);
  }

private:
  struct statement_guard {
    explicit statement_guard(statement_cache_proxy &statement_proxy) : proxy(statement_proxy) {}
    ~statement_guard() {
      proxy.unlock();
    }

    statement_cache_proxy &proxy;
  };

private:
  std::atomic_bool locked_{false};
  connection_pool &pool_;
  size_t connection_id_{};
  retry_config config_{};
  utils::message_bus &bus_;
};
}

statement_cache::statement_cache(utils::message_bus &bus, connection_pool &pool, const size_t max_size)
: max_size_(max_size)
, pool_(pool)
, dialect_(backend_provider::instance().connection_dialect(pool_.info().type))
, bus_(bus) {}

utils::result<statement, utils::error> statement_cache::acquire(const query_context& ctx) {
  std::unique_lock lock(mutex_);
  // hash statement
  const auto now = std::chrono::steady_clock::now();
  // Found in cache. Move it to of the LRU list
  if (const auto it = cache_map_.find(ctx.sql_hash); it != cache_map_.end()) {
    usage_list_.splice(usage_list_.begin(), usage_list_, it->second.position);
    it->second.last_access = now;
    bus_.publish<statement_accessed_event>({ctx.sql, std::chrono::steady_clock::now()});
    return utils::ok(it->second.stmt);
  }
  // Prepare a new statement
  // acquire pool connection
  size_t id{};
  std::unique_ptr<statement_impl> stmt;
  {
    const auto conn = pool_.acquire();
    auto result = conn->perform_prepare(ctx);
    if (!result) {
      return utils::failure(utils::error{error_code::PrepareFailed, std::string("Failed to prepare")});
    }
    id = conn.id().value();
    stmt = result.release();
  }
  // If cache max size reached ensure space
  if (cache_map_.size() >= max_size_) {
    const auto& key_to_remove = usage_list_.back();
    cache_map_.erase(key_to_remove);
    usage_list_.pop_back();
    bus_.publish<statement_evicted_event>({ctx.sql, std::chrono::steady_clock::now()});
  }

  usage_list_.push_front(ctx.sql_hash);
  const auto it = cache_map_.insert({
    ctx.sql_hash,
    {statement{
      std::make_shared<statement_cache_proxy>(bus_, std::move(stmt), pool_, id)},
      std::chrono::steady_clock::now(),
      usage_list_.begin()}
  }).first;
  bus_.publish<statement_added_event>({ctx.sql, std::chrono::steady_clock::now()});

  return utils::ok(it->second.stmt);
}

size_t statement_cache::size() const {
  return cache_map_.size();
}
size_t statement_cache::capacity() const {
  return max_size_;
}

bool statement_cache::empty() const {
  return cache_map_.empty();
}

connection_pool& statement_cache::pool() const {
  return pool_;
}
}
