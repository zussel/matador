#ifndef QUERY_CONNECTION_POOL_HPP
#define QUERY_CONNECTION_POOL_HPP

#include "matador/query/connection.hpp"
#include "matador/query/connection_info.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <optional>
#include <unordered_map>

namespace matador::query {
class connection_pool;

struct identifiable_connection {
  identifiable_connection(size_t id, connection conn);
  size_t id{};
  connection conn;
};

class connection_ptr {
public:
  connection_ptr(identifiable_connection *c, connection_pool *pool);
  ~connection_ptr();
  connection_ptr(const connection_ptr &) = delete;
  connection_ptr& operator=(const connection_ptr &) = delete;
  connection_ptr(connection_ptr &&x) noexcept;

  connection_ptr& operator=(connection_ptr &&x) noexcept;

  connection* operator->() const;
  connection& operator*() const;

  [[nodiscard]] std::optional<size_t> id() const;
  [[nodiscard]] bool valid() const;

  void release() const;

private:
  friend class connection_pool;

  identifiable_connection *connection_{};
  connection_pool *pool_{};
};

class connection_pool {
public:
  connection_pool(const std::string &dns, size_t count);
  connection_pool(const std::string &dns, size_t count, std::function<connection(const connection_info &)> &&connection_creator);

  connection_ptr acquire();
  connection_ptr try_acquire();
  connection_ptr try_acquire(size_t id);
  connection_ptr acquire(size_t id, std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));

  void release(identifiable_connection *c);
  void release(connection_ptr &c);

  std::size_t size() const;
  std::size_t idle() const;
  std::size_t inuse() const;

  const connection_info &info() const;

private:
  connection_ptr get_next_connection();

private:
  mutable std::mutex mutex_;
  std::condition_variable cv;
  std::vector<identifiable_connection> connection_repo_;
  using pointer = identifiable_connection*;
  using connection_map = std::unordered_map<size_t, pointer>;
  connection_map inuse_connections_;
  connection_map idle_connections_;

  const connection_info info_;
  std::function<connection(const connection_info &)> connection_creator_;
};

}

#endif //QUERY_CONNECTION_POOL_HPP
