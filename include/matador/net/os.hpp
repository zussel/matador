#ifndef MATADOR_NET_OS_HPP
#define MATADOR_NET_OS_HPP

#include "matador/net/export.hpp"

#include <cstdio>

namespace matador {
namespace net {

OOS_NET_API void init();
OOS_NET_API void cleanup();

}

namespace os {

OOS_NET_API int inet_pton(int af, const char *src, void *dst);
OOS_NET_API const char* inet_ntop(int af, const void* src, char* dst, size_t size);

enum class shutdown_type
{
    READ = 0,
    WRITE = 1,
    READ_WRITE = 2
};

OOS_NET_API int shutdown(int fd, shutdown_type type);
OOS_NET_API int close(int fd);

}
}
#endif //MATADOR_NET_OS_HPP
