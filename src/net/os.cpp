#include "matador/net/os.hpp"

#ifdef _WIN32
#else
#include <arpa/inet.h>
#endif

namespace matador {
namespace os {

int inet_pton(int af, const char *src, void *dst)
{
#ifdef _WIN32
#else
  return ::inet_pton(af, src, dst);
#endif
}


}
}
