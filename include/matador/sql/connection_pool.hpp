#ifndef QUERY_CONNECTION_POOL_HPP
#define QUERY_CONNECTION_POOL_HPP

#include "matador/sql/connection_info.hpp"

#include <chrono>
#include <mutex>
#include <string>
#include <optional>
#include <condition_variable>
#include <thread>
#include <unordered_map>

namespace matador::sql {

template < class Connection >
class connection_pool;

template < class Connection >
using IdConnection = std::pair<size_t, Connection>;

template < class Connection >
class connection_ptr
{
public:
  connection_ptr(IdConnection<Connection> *c, connection_pool<Connection> *pool)
  : connection_(c), pool_(pool) {}
  ~connection_ptr();
  connection_ptr(const connection_ptr &) = delete;
  connection_ptr& operator=(const connection_ptr &) = delete;
  connection_ptr(connection_ptr &&x) noexcept
  : connection_(x.connection_)
  , pool_(x.pool_)
  {
    x.connection_ = nullptr;
    x.pool_ = nullptr;
  }
  connection_ptr& operator=(connection_ptr &&x) noexcept
  {
    if (this == &x) {
      return *this;
    }

    std::swap(connection_, x.connection_);
    std::swap(pool_, x.pool_);

    return *this;
  }

  Connection* operator->() { return &connection_->second; }
  Connection& operator*() { return connection_->second; }

  [[nodiscard]] std::optional<size_t> id() const
  {
    if (connection_) {
      return connection_->first;
    } else {
      return std::nullopt;
    }
  }
  [[nodiscard]] bool valid() const { return connection_ != nullptr; }

private:
  friend class connection_pool<Connection>;

  IdConnection<Connection> *connection_{};
  connection_pool<Connection> *pool_{};
};

template < class Connection >
class connection_pool
{
public:
  using connection_pointer = connection_ptr<Connection>;

public:
  connection_pool(const std::string &dns, size_t count)
  : info_(connection_info::parse(dns)) {
    connection_repo_.reserve(count);
    while (count) {
      connection_repo_.emplace_back(count, info_);
      auto &conn = connection_repo_.back();
      idle_connections_.emplace(conn.first, &conn);
      conn.second.open();
      --count;
    }
  }

  connection_pointer acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (idle_connections_.empty()) {
      cv.wait(lock);
    }

    return get_next_connection();
  }

  connection_pointer try_acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (idle_connections_.empty()) {
      return {nullptr, this};
    }

    return get_next_connection();
  }

  connection_pointer acquire(size_t id) {
    using namespace std::chrono_literals;
    pointer next_connection{nullptr};
    auto try_count{0};
    std::unique_lock<std::mutex> lock(mutex_);

    do {
      if (auto it = idle_connections_.find(id); it != idle_connections_.end()) {
        next_connection = it->second;
        auto node = idle_connections_.extract(it);
        inuse_connections_.insert(std::move(node));
      } else {
        lock.unlock();
        std::this_thread::sleep_for(100ms);
        lock.lock();
      }
    } while(try_count++ < 5);

    return {next_connection, this};
  }

  void release(IdConnection<Connection> *c) {
    if (c == nullptr) {
      return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if (auto it = inuse_connections_.find(c->first); it != inuse_connections_.end()) {
      auto node = inuse_connections_.extract(it);
      idle_connections_.insert(std::move(node));
    }
  }

  void release(connection_ptr<Connection> &c) {
    release(c.connection_);
    c.connection_ = nullptr;
  }
  
  std::size_t size() const { return connection_repo_.size(); }
  std::size_t idle() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return idle_connections_.size();
  }
  std::size_t inuse() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return inuse_connections_.size();
  }

  const connection_info &info() const {
    return info_;
  }

private:
  connection_pointer get_next_connection() {
    pointer next_connection{nullptr};
    for (auto &item : idle_connections_) {
      next_connection = item.second;
      auto node = idle_connections_.extract(item.first);
      inuse_connections_.insert(std::move(node));
      break;
    }
    return {next_connection, this};
  }
private:
  mutable std::mutex mutex_;
  std::condition_variable cv;
  std::vector<IdConnection<Connection>> connection_repo_;
  using pointer = IdConnection<Connection>*;
  using connection_map = std::unordered_map<size_t, pointer>;
  connection_map inuse_connections_;
  connection_map idle_connections_;

  const connection_info info_;
};

template<class Connection>
connection_ptr<Connection>::~connection_ptr() {
  pool_->release(connection_);
}

}

#endif //QUERY_CONNECTION_POOL_HPP
