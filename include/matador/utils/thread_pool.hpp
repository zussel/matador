#ifndef MATADOR_THREAD_POOL_HPP
#define MATADOR_THREAD_POOL_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

namespace matador {

class OOS_UTILS_API thread_pool
{
private:
  thread_pool(const thread_pool&);
  thread_pool& operator=(const thread_pool&);

public:
  explicit thread_pool(std::size_t size);
  ~thread_pool();

  /**
   * Push a task into the thread pool
   * to be executed once a thread
   * is available
   */
  template < typename F >
  void schedule(F func) {
    const std::lock_guard<std::mutex> l(mutex_);
    tasks_.push_back(func);
    condition_task_.notify_one();
  }

  std::size_t size() const;

  void shutdown();
  void wait();

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
