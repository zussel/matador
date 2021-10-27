#ifndef MATADOR_NETUTILS_HPP
#define MATADOR_NETUTILS_HPP

#ifndef _WIN32
#include <unistd.h>
#endif

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
#ifdef _WIN32
    ::Sleep(500);
#else
    ::usleep(500);
#endif
  }

  return service.is_running() == running;
}

}
}

#endif //MATADOR_NETUTILS_HPP
