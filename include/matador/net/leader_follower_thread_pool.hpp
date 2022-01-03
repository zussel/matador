#ifndef MATADOR_LEADER_FOLLOWER_THREAD_POOL_HPP
#define MATADOR_LEADER_FOLLOWER_THREAD_POOL_HPP

#include "matador/utils/export.hpp"

#include "matador/logger/log_manager.hpp"

#include <memory>
#include <thread>
#include <vector>
#include <functional>
#include <deque>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace matador {

class OOS_UTILS_API leader_follower_thread_pool
{
private:
  leader_follower_thread_pool(const leader_follower_thread_pool &);

  leader_follower_thread_pool &operator=(const leader_follower_thread_pool &);

public:
  template<typename F>
  leader_follower_thread_pool(std::size_t size, F join_func)
    : num_threads_(size), join_(join_func)
    , log_(matador::create_logger("LFThreadPool"))
    , follower_(size) {}

  ~leader_follower_thread_pool();

  void start();

  void stop();

  void promote_new_leader();

  std::size_t size() const;

  void shutdown();

  std::thread::id leader();

  std::size_t num_follower() const;

private:
  /*
   * wait for a task to execute
   */
  void execute();

private:
  typedef std::unique_ptr<std::thread> thread_ptr;
  typedef std::vector<thread_ptr> thread_vector_t;
  typedef std::function<void()> join_func_t;

  std::size_t num_threads_{};

  std::thread::id leader_{};
  std::thread::id null_id{};

  join_func_t join_;
  thread_vector_t threads_;

  std::mutex mutex_;
  std::condition_variable condition_task_;
  std::condition_variable condition_synchronizer_;

  bool signal_ready_ = false;

  bool is_running_ = true;

  matador::logger log_;

  std::atomic_size_t follower_{};
};

}

#endif //MATADOR_LEADER_FOLLOWER_THREAD_POOL_HPP
