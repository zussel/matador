#include "matador/net/error.hpp"
#include "matador/utils/os.hpp"

#include <stdexcept>

#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <netdb.h>
#endif

namespace matador {
namespace detail {

void throw_logic_error(const char* msg)
{
  throw std::logic_error(msg);
}

void throw_logic_error_with_errno(const char* msg, int err)
{
  char error_buffer[1024];
  os::strerror(err, error_buffer, 1024);

  char message_buffer[1024];
  os::sprintf(message_buffer, 1024, msg, error_buffer);
  throw std::logic_error(message_buffer);
}

void throw_logic_error_with_gai_errno(const char* msg, int err)
{
  char message_buffer[1024];
  os::sprintf(message_buffer, 1024, msg, gai_strerror(err));
  throw std::logic_error(message_buffer);
}

}
}