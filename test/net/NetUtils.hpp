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

}
}

#endif //MATADOR_NETUTILS_HPP
