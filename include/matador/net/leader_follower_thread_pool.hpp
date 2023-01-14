#ifndef MATADOR_LEADER_FOLLOWER_THREAD_POOL_HPP
#define MATADOR_LEADER_FOLLOWER_THREAD_POOL_HPP

#include "matador/net/export.hpp"

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

/**
 * This thread pool class implements the
 * leader follower pattern.
 */
class OOS_NET_API leader_follower_thread_pool
{
private:
  leader_follower_thread_pool(const leader_follower_thread_pool &);

  leader_follower_thread_pool &operator=(const leader_follower_thread_pool &);

public:
  /**
   * Creates a new leader follower thread pool instance
   * with the given thread pool size and given join
   * function.
   *
   * @tparam F Type of join function
   * @param size Number of threads
   * @param join_func Join function.
   */
  template<typename F>
  leader_follower_thread_pool(std::size_t size, F join_func)
    : num_threads_(size), join_(join_func)
    , log_(matador::create_logger("LFThreadPool"))
    , follower_(size) {}

  ~leader_follower_thread_pool();

  /**
   * Starts the thread pool.
   */
  void start();

  /**
   * Stops the thread pool.
   */
  void stop();

  /**
   * Promotes the next new leading thread.
   */
  void promote_new_leader();

  /**
   * Returns number of threads.
   *
   * @return Number of threads.
   */
  std::size_t size() const;

  /**
   * Shuts the thread pool down.
   */
  void shutdown();

  /**
   * Returns the thread id of the current
   * leading thread.
   *
   * @return Thread id of the leading thread.
   */
  std::thread::id leader();

  /**
   * Returns the current number of
   * thread followers.
   *
   * @return Number of follower threads.
   */
  std::size_t num_follower() const;

  /**
   * Returns true if the thread pool is running.
   *
   * @return True if thread pool is running.
   */
  bool is_running() const;

private:
  /*
   * wait for a task to execute
   */
  void execute();

private:
  typedef std::vector<std::thread> thread_vector_t;
  typedef std::function<void()> join_func_t;

  std::size_t num_threads_{};

  std::thread::id leader_{};
  std::thread::id null_id{};

  join_func_t join_;
  thread_vector_t threads_;

  std::mutex mutex_;
  std::condition_variable condition_task_;
  std::condition_variable condition_synchronizer_;

  std::atomic_bool signal_ready_ { false };
  std::atomic_bool signal_shutdown_ { false };

  std::atomic_bool is_running_ { false };

  matador::logger log_;

  std::atomic_size_t follower_{};
};

}

#endif //MATADOR_LEADER_FOLLOWER_THREAD_POOL_HPP
