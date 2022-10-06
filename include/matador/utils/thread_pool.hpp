#ifndef MATADOR_THREAD_POOL_HPP
#define MATADOR_THREAD_POOL_HPP

#include "matador/utils/export.hpp"

#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

namespace matador {

/**
 * Simple thread pool class. The pool
 * consists of a given number of threads and
 * a queue of tasks to be executed.
 */
class OOS_UTILS_API thread_pool
{
private:
  thread_pool(const thread_pool&);
  thread_pool& operator=(const thread_pool&);

public:
  /**
   * Creating a new thread pool with given
   * numbers of threads.
   *
   * @param size Number of provided threads.
   */
  explicit thread_pool(std::size_t size);
  ~thread_pool();

  /**
   * Push a task into the thread pool
   * to be executed once a thread is available
   *
   * @tparam F Type of threaded function
   * @param func Function to be executed in next available thread.
   */
  template < typename F >
  void schedule(F func) {
    const std::lock_guard<std::mutex> l(mutex_);
    tasks_.push_back(func);
    condition_task_.notify_one();
  }

  /**
   * Adds a new task to the thread queue.
   */
  void add_task();

  /**
   * Returns the number of available threads.
   *
   * @return Number of threads.
   */
  std::size_t size() const;

  /**
   * Shuts the thread pool down.
   */
  void shutdown();

  /**
   * Waits until all threads are finished.
   */
  void wait();

  /**
   * Returns the number of pending tasks.
   *
   * @return Number of pending tasks.
   */
  std::size_t pending();

private:
  /*
   * wait for a task to execute
   */
  void execute();

private:
  typedef std::unique_ptr<std::thread> thread_ptr;
  typedef std::vector<thread_ptr> thread_vector_t;
  typedef std::function<void()> task_func_t;
  typedef std::deque<task_func_t> task_deque_t;

  std::atomic_uint busy_;

  task_deque_t tasks_;
  thread_vector_t threads_;

  std::mutex mutex_;
  std::condition_variable condition_task_;
  std::condition_variable condition_finished_;

  bool running = true;
};

}

#endif //MATADOR_THREAD_POOL_HPP
