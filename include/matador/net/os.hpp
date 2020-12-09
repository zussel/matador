#ifndef MATADOR_NET_OS_HPP
#define MATADOR_NET_OS_HPP

#include <cstdio>

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
#define OOS_NET_API __declspec(dllexport)
#define EXPIMP_NET_TEMPLATE
#else
#define OOS_NET_API __declspec(dllimport)
#define EXPIMP_NET_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

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
