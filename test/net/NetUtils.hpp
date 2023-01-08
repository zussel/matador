#ifndef MATADOR_NETUTILS_HPP
#define MATADOR_NETUTILS_HPP

#include <thread>

namespace detail {
namespace utils {

template < class T >
bool wait_until(T &service, bool running, int retries = 10);

template < class T >
bool wait_until_running(T &service, int retries = 10)
{
  return wait_until(service, true, retries);
}

template < class T >
bool wait_until_stopped(T &service, int retries = 10)
{
  return wait_until(service, false, retries);
}

template < class T >
bool wait_until(T &service, bool running, int retries)
{
  while (service.is_running() != running && retries-- > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  return service.is_running() == running;
}

class ThreadRunner
{
public:
  template< typename RunFunc, typename ShutdownFunc >
  ThreadRunner(RunFunc &&run_func, ShutdownFunc &&shutdown_func)
  : thread_(std::forward<RunFunc>(run_func))
  , shutdown_func_(std::forward<ShutdownFunc>(shutdown_func))
  {}
  ~ThreadRunner() {
    shutdown_func_();

    if (thread_.joinable()) {
      thread_.join();
    }
  }

private:
  std::thread thread_;
  std::function<void()> shutdown_func_;
};

}
}

#endif //MATADOR_NETUTILS_HPP
