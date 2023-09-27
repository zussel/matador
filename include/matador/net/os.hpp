#ifndef MATADOR_NET_OS_HPP
#define MATADOR_NET_OS_HPP

#include "matador/net/export.hpp"

#include <cstdio>

#if _WIN32
#include <WinSock2.h>
#endif

#ifdef _WIN32
  using socket_type = SOCKET;
#else
  using socket_type = int;
#endif

namespace matador {
namespace net {

OOS_NET_API void init();
OOS_NET_API void cleanup();

}

bool is_valid_socket(socket_type fd);

namespace os {

OOS_NET_API int inet_pton(int af, const char *src, void *dst);
OOS_NET_API const char* inet_ntop(int af, const void* src, char* dst, size_t size);

enum class shutdown_type
{
    READ = 0,
    WRITE = 1,
    READ_WRITE = 2
};

OOS_NET_API int shutdown(socket_type fd, shutdown_type type);
OOS_NET_API int close(socket_type fd);

}
}
#endif //MATADOR_NET_OS_HPP
