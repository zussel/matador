#include "matador/utils/leader_follower_thread_pool.hpp"

#include "matador/utils/thread_helper.hpp"

#include <algorithm>

namespace matador {

leader_follower_thread_pool::~leader_follower_thread_pool() {
  shutdown();
}

void leader_follower_thread_pool::start() {
  is_running_ = true;
  for (std::size_t i = 0; i < num_threads_; ++i) {
    thread_ptr t(new std::thread([this] { execute(); }));
    threads_.push_back(std::move(t));
  }
  log_.info("thread pool started with %d threads", num_threads_);
}

void leader_follower_thread_pool::stop() {
  is_running_ = false;
}

void leader_follower_thread_pool::promote_new_leader() {
  std::lock_guard<std::mutex> l(mutex_);

  if (leader_ != std::this_thread::get_id()) {
    log_.info("leader isn't current thread, skipping");
    return;
  }

  leader_ = null_id;
  log_.info("promoting new leader");

  signal_ready_ = true;
  condition_synchronizer_.notify_one();
}

std::size_t leader_follower_thread_pool::size() const {
  return threads_.size();
}

void leader_follower_thread_pool::shutdown() {
  {
    const std::lock_guard<std::mutex> l(mutex_);
    if (!is_running_) {
      return;
    }
    stop();
    log_.info("shutting down; notifying all tasks");
    signal_ready_ = true;
    condition_synchronizer_.notify_all();
  }

  std::for_each(threads_.begin(), threads_.end(), [](thread_vector_t::reference item) {
    if (item->joinable()) {
      item->join();
    }
  });
}

std::thread::id leader_follower_thread_pool::leader() {
  const std::lock_guard<std::mutex> l(mutex_);
  return leader_;
}

std::size_t leader_follower_thread_pool::num_follower() const {
  return follower_;
}

void leader_follower_thread_pool::execute() {
  std::unique_lock<std::mutex> l(mutex_);
  while (is_running_) {
    while (leader_ != null_id) {
      log_.info("waiting for synchronizer (leader_ %d)", acquire_thread_index(leader_));
      condition_synchronizer_.wait(l, [this]() { return signal_ready_; });
      signal_ready_ = false;
      log_.info("is running: %d", is_running_);
      if (!is_running_) {
        log_.info("task %d not running; returning", acquire_thread_index(std::this_thread::get_id()));
        return;
      }
      log_.info("got signal");
    }

    leader_ = std::this_thread::get_id();
    log_.info("new leader <%d>", acquire_thread_index(leader_));

    l.unlock();

    join_();

    l.lock();
  }
  log_.info("finished task %d", acquire_thread_index(std::this_thread::get_id()));
}
}