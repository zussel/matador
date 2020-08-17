#include "matador/net/socket.hpp"
#include "matador/net/os.hpp"
#include "matador/net/error.hpp"

#include <string.h>

namespace matador {

#define throw_logic_error(msg) \
  do { \
    std::stringstream str; \
    str << msg; \
    throw std::logic_error(str.str()); \
  } while(false);

template < class P >
socket_base<P>::socket_base(const protocol_type &protocol)
{
  open(protocol);
}

template < class P >
socket_base<P>::socket_base(const peer_type &peer)
{
  open(peer.protocol());
}

template < class P >
int socket_base<P>::open(const protocol_type &protocol)
{
  return open(protocol.family(), protocol.type(), protocol.protocol());
}

template < class P >
void socket_base<P>::close()
{
  os::shutdown(sock_, os::shutdown_type::READ_WRITE);
  sock_ = 0;
}

template < class P >
bool socket_base<P>::is_open() const
{
  return sock_ > 0;
}

template < class P >
int socket_base<P>::release()
{
  int tmp_fd = sock_;
  sock_ = 0;
  return tmp_fd;
}

template < class P >
int socket_base<P>::connect(const typename protocol_type::peer &p)
{
  return ::connect(sock_, p.data(), p.size());
}

template < class P >
void socket_base<P>::non_blocking(bool nb)
{
#ifdef WIN32
  unsigned long nonblock = nb ? 0 : 1;
  // fcntl doesn't do the right thing, but the simular ioctl does
  // warning: is that still true? and does it the right thing for
  // set blocking as well?
  if (ioctlsocket(sock_, FIONBIO, &nonblock) != 0) {
    throw std::logic_error("ioctlsocket: couldn't set flags");
  }
  is_nonblocking_ = nb;
#else
  int val = fcntl(sock_, F_GETFL, 0);
  if (val < 0) {
    throw std::logic_error("fcntl: couldn't get flags");
  }

  int flag = (nb ? O_NONBLOCK : 0);
  if (fcntl(sock_, F_SETFL, val | flag) < 0) {
    std::string err(strerror(errno));
    throw std::logic_error("fcntl: couldn't set flags (" + err + ")");
  }
#endif
}

template < class P >
bool socket_base<P>::non_blocking() const
{
#ifdef _WIN32
  return is_nonblocking_;
#else
  int val = fcntl(sock_, F_GETFL, 0);
  if (val < 0) {
    std::string err(strerror(errno));
    throw std::logic_error("fcntl: couldn't get flags (" + err + ")");
  }
  return (val & O_NONBLOCK) > 0;
#endif
}

template < class P >
void socket_base<P>::cloexec(bool nb)
{
#ifdef WIN32
  unsigned long cloexec = 1;
  // fcntl doesn't do the right thing, but the simular ioctl does
  // warning: is that still true? and does it the right thing for
  // set blocking as well?
  if (ioctlsocket(sock_, FIONBIO, &cloexec) != 0) {
    throw std::logic_error("ioctlsocket: couldn't set flags");
  }
#else
  int val = fcntl(sock_, F_GETFL, 0);
  if (val < 0) {
    throw std::logic_error("fcntl: couldn't get flags");
  }

  int flag = (nb ? FD_CLOEXEC : 0);
  if (fcntl(sock_, F_SETFL, val | flag) < 0) {
    std::string err(strerror(errno));
    throw std::logic_error("fcntl: couldn't set flags (" + err + ")");
  }
#endif
}

template < class P >
bool socket_base<P>::cloexec() const
{
  int val = fcntl(sock_, F_GETFL, 0);
  if (val < 0) {
    std::string err(strerror(errno));
    throw std::logic_error("fcntl: couldn't get flags (" + err + ")");
  }
  return (val & FD_CLOEXEC) > 0;
}

template < class P >
int socket_base<P>::options(int name, bool value)
{
  int flag = (value ? 1 : 0);
  return setsockopt(sock_, IPPROTO_TCP, name, &flag, sizeof(flag));
}

template < class P >
int socket_base<P>::id() const
{
  return sock_;
}

template < class P >
void socket_base<P>::assign(int sock)
{
  if (is_open()) {
    throw std::logic_error("couldn't assign: socket already opened");
  }

  struct sockaddr_in addr{};
  socklen_t addr_size = sizeof(struct sockaddr_in);
  if (getpeername(sock, (struct sockaddr *)&addr, &addr_size) == 0) {
    //char *clientip = new char[20];
    char s[INET6_ADDRSTRLEN];
    os::inet_ntop(addr.sin_family, &addr.sin_addr, s, sizeof s);
#ifdef _WIN32
//    strcpy_s(clientip, 20, s);
#else
//    strcpy(clientip, s);
#endif
  }
  sock_ = sock;
}

template < class P >
int socket_base<P>::open(int family, int type, int protocol)
{
  sock_ = ::socket(family, type, protocol);
  return sock_;
}

}