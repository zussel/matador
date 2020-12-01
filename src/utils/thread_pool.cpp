#include <thread>
#include <algorithm>
#include "matador/utils/thread_pool.hpp"

namespace matador {

thread_pool::thread_pool(std::size_t size)
  : busy_(0U)
{
  for (std::size_t i = 0; i < size; ++i) {
    thread_ptr t(new std::thread([this] { execute(); }));
    threads_.push_back(std::move(t));
  }
}

thread_pool::~thread_pool()
{
  shutdown();
}

std::size_t thread_pool::size() const
{
  return threads_.size();
}

void thread_pool::shutdown()
{
  {
    const std::lock_guard<std::mutex> l(mutex_);
    if (!running) {
      return;
    }
    running = false;
    condition_task_.notify_all();
  }

  std::for_each(threads_.begin(), threads_.end(), [](thread_vector_t::reference item) {
    if (item->joinable()) {
      item->join();
    }
  });
}

void thread_pool::wait()
{
  std::unique_lock<std::mutex> l(mutex_);
  while (!tasks_.empty() || busy_ > 0) {
    condition_finished_.wait(l);
  }
}

std::size_t thread_pool::pending()
{
  const std::lock_guard<std::mutex> l(mutex_);
  return tasks_.size();
}

void thread_pool::execute()
{
  do {
    std::unique_lock<std::mutex> l(mutex_);
    condition_task_.wait(l, [this]() {
      return !running ||!tasks_.empty();
    });
    if (!running) {
      break;
    } else if (!tasks_.empty()) {
      ++busy_;
      task_func_t func = tasks_.front();
      tasks_.pop_front();
      l.unlock();
      func();
      --busy_;
      condition_finished_.notify_one();
    }
  } while(true);
}

}