#include "matador/net/leader_follower_thread_pool.hpp"

#include "matador/utils/thread_helper.hpp"

#include <algorithm>

namespace matador {

leader_follower_thread_pool::~leader_follower_thread_pool()
{
  shutdown();
}

void leader_follower_thread_pool::start()
{
  is_running_ = true;
  for (std::size_t i = 0; i < num_threads_; ++i) {
    threads_.emplace_back([this] { execute(); });
  }
  log_.info("thread pool started with %d threads", num_threads_);
}

void leader_follower_thread_pool::stop()
{
  is_running_ = false;
}

void leader_follower_thread_pool::promote_new_leader()
{
  std::lock_guard<std::mutex> l(mutex_);

  if (leader_ != std::this_thread::get_id()) {
    return;
  }

  leader_ = null_id;

  signal_ready_ = true;
  condition_synchronizer_.notify_one();
}

std::size_t leader_follower_thread_pool::size() const
{
  return threads_.size();
}

void leader_follower_thread_pool::shutdown()
{
  {
    const std::lock_guard<std::mutex> l(mutex_);
    if (!is_running_) {
      return;
    }
    stop();
    log_.info("shutting down; notifying all tasks");
    signal_shutdown_ = true;
    condition_synchronizer_.notify_all();
  }

  std::for_each(threads_.begin(), threads_.end(), [](thread_vector_t::reference item) {
    if (item.joinable()) {
      item.join();
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

bool leader_follower_thread_pool::is_running() const
{
  return is_running_;
}

void leader_follower_thread_pool::execute() {
  std::unique_lock<std::mutex> l(mutex_);
  while (is_running_) {
    while (leader_ != null_id) {
//      log_.info("thread <%d> waiting for synchronizer (leader %d)",
//                acquire_thread_index(std::this_thread::get_id()),
//                acquire_thread_index(leader_));
      condition_synchronizer_.wait(l, [this]() { return signal_ready_ || signal_shutdown_; });
      signal_ready_ = false;
      if (!is_running_) {
        return;
      }
    }

//    log_.info("new leader <%d> (thread <%d> is now follower)",acquire_thread_index(std::this_thread::get_id()) , acquire_thread_index(leader_));
    leader_ = std::this_thread::get_id();

    l.unlock();

    join_();

//    log_.info("thread <%d> finished work", acquire_thread_index(std::this_thread::get_id()));
    l.lock();
  }
}
}