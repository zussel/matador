#include "matador/query/connection_pool.hpp"

#include <thread>
#include <utility>

namespace matador::query {
identifiable_connection::identifiable_connection(const size_t id, connection conn)
: id(id)
, conn(std::move(conn)){}

connection_ptr::~connection_ptr() {
  release();
}

connection_ptr::connection_ptr(identifiable_connection* c, connection_pool* pool)
: connection_(c)
, pool_(pool) {}

connection_ptr::connection_ptr(connection_ptr&& x) noexcept
: connection_(x.connection_)
, pool_(x.pool_) {
  x.connection_ = nullptr;
  x.pool_ = nullptr;
}

connection_ptr& connection_ptr::operator=(connection_ptr&& x) noexcept {
  if (this == &x) {
    return *this;
  }

  std::swap(connection_, x.connection_);
  std::swap(pool_, x.pool_);

  return *this;
}

connection* connection_ptr::operator->() const {
  return &connection_->conn;
}

connection& connection_ptr::operator*() const {
  return connection_->conn;
}

std::optional<size_t> connection_ptr::id() const {
  if (connection_) {
    return connection_->id;
  }
  return std::nullopt;
}

bool connection_ptr::valid() const {
  return connection_ != nullptr;
}

void connection_ptr::release() const {
  pool_->release(connection_);
}

connection_pool::connection_pool(const std::string& dns, const size_t count)
: connection_pool(dns, count, [](const connection_info &info) { return connection{info}; })
{}

connection_pool::connection_pool(const std::string &dns, size_t count, std::function<connection(const connection_info &)> &&connection_creator)
: info_(connection_info::parse(dns))
, connection_creator_(std::move(connection_creator)) {
  connection_repo_.reserve(count);
  while (count) {
    connection_repo_.emplace_back(count, connection_creator_(info_));
    auto &conn = connection_repo_.back();
    idle_connections_.emplace(conn.id, &conn);
    if (const auto result = conn.conn.open(); !result) {
      throw std::runtime_error("Failed to open connection");
    }
    --count;
  }
}

connection_ptr connection_pool::acquire() {
  std::unique_lock lock(mutex_);
  if (!cv.wait_for(lock,
                         std::chrono::seconds(30),
                         [this] { return !idle_connections_.empty(); })) {
    return {nullptr, this};
  }
  // while (idle_connections_.empty()) {
    // cv.wait(lock);
  // }

  return get_next_connection();
}

connection_ptr connection_pool::try_acquire() {
  std::unique_lock lock(mutex_);
  if (idle_connections_.empty()) {
    return {nullptr, this};
  }

  return get_next_connection();
}

connection_ptr connection_pool::try_acquire(const size_t id) {
  std::unique_lock lock(mutex_);
  const auto it = idle_connections_.find(id);
  if (it == idle_connections_.end()) {
    return {nullptr, this};
  }
  return {it->second, this};
}

connection_ptr connection_pool::acquire(const size_t id, const std::chrono::milliseconds timeout) {
  std::unique_lock lock(mutex_);

  if (!cv.wait_for(lock, timeout, [this, id] {
    return idle_connections_.find(id) != idle_connections_.end();
  })) {
    return {nullptr, this};
  }

  const auto it = idle_connections_.find(id);
  auto next_connection = it->second;
  auto node = idle_connections_.extract(it);
  inuse_connections_.insert(std::move(node));
  return {next_connection, this};
}

void connection_pool::release(identifiable_connection* c) {
  if (c == nullptr) {
    return;
  }
  std::unique_lock lock(mutex_);
  if (const auto it = inuse_connections_.find(c->id); it != inuse_connections_.end()) {
    auto node = inuse_connections_.extract(it);
    idle_connections_.insert(std::move(node));
    cv.notify_one();
  }
}

void connection_pool::release(connection_ptr& c) {
  release(c.connection_);
  c.connection_ = nullptr;
}

std::size_t connection_pool::size() const {
  return connection_repo_.size();
}

std::size_t connection_pool::idle() const {
  std::lock_guard guard(mutex_);
  return idle_connections_.size();
}

std::size_t connection_pool::inuse() const {
  std::lock_guard guard(mutex_);
  return inuse_connections_.size();
}

const connection_info& connection_pool::info() const {
  return info_;
}

connection_ptr connection_pool::get_next_connection() {
  pointer next_connection{nullptr};
    for (auto & [id, conn] : idle_connections_) {
    next_connection = conn;
    auto node = idle_connections_.extract(id);
    inuse_connections_.insert(std::move(node));
    break;
  }
  return {next_connection, this};
}
}
