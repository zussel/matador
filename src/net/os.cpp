#include "matador/net/os.hpp"

#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace matador {
namespace net {

void init()
{
#ifdef _WIN32
  WSADATA wsaData;   // if this doesn't work
  //WSAData wsaData; // then try this instead

  // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:

  if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
      fprintf(stderr, "WSAStartup failed.\n");
      exit(1);
  }
#endif
}

void cleanup()
{
#ifdef _WIN32
  WSACleanup();
#endif
}

}
namespace os {

int inet_pton(int af, const char *src, void *dst)
{
#ifdef _WIN32
  return ::InetPton(af, src, dst);
#else
  return ::inet_pton(af, src, dst);
#endif
}

const char* inet_ntop(int af, const void* src, char* dst, size_t size)
{
#ifdef _WIN32
    return ::InetNtop(af, const_cast<void*>(src), dst, size);
#else
    return ::inet_ntop(af, src, dst, size);
#endif
}

int close(int fd)
{
#ifdef _WIN32
  return ::closesocket(fd);
#else
  return ::close(fd);
#endif
}

int shutdown(int fd, shutdown_type type)
{
#ifdef _WIN32
    return ::shutdown(fd, static_cast<int>(type));
#else
    return ::shutdown(fd, static_cast<int>(type));
#endif
}

}
}
