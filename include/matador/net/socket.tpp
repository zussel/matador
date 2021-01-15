#include "matador/net/socket.hpp"
#include "matador/net/socket_stream.hpp"
#include "matador/net/os.hpp"
#include "matador/net/error.hpp"
#include "matador/net/ip.hpp"

#include <cstring>

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
  if (sock_ <= 0) {
    return;
  }
  //os::shutdown(sock_, os::shutdown_type::READ_WRITE);
  os::close(sock_);
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
bool socket_base<P>::connect(const typename protocol_type::peer &p)
{
  return ::connect(sock_, p.data(), p.size()) == 0;
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
bool socket_base<P>::options(int name, bool value)
{
  const char flag = static_cast<char>(value ? 1 : 0);
  return setsockopt(sock_, IPPROTO_TCP, name, &flag, sizeof(flag)) == 0;
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

template < class P >
int connect(socket_stream<P> &stream, const char* hostname, unsigned short port)
{
  char portstr[6];
  sprintf(portstr, "%d", port);
//    const char* portname = "daytime";
  struct addrinfo hints = {};
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo* res = nullptr;
  struct addrinfo* head = nullptr;
  int err = getaddrinfo(hostname, portstr, &hints, &res);
  if (err != 0) {
    detail::throw_logic_error_with_gai_errno("failed to resolve local socket address: %s", err);
  }

  head = res;

  int connfd = 0;
  int ret = 0;
  do {
    connfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connfd < 0) {
      // error, try next one
      continue;
    }

    ret = ::connect(connfd, res->ai_addr, res->ai_addrlen);
    if (ret == 0) {
      // success
      stream.assign(connfd);
      break;
//      } else {
//        throw_logic_error("couldn't execute: " << strerror(errno));
    }

    // bind error, close and try next one
    os::shutdown(connfd, os::shutdown_type::READ_WRITE);
  } while ( (res = res->ai_next) != nullptr);

  if (res == nullptr) {
    throw_logic_error("couldn't execute to " << hostname << ":" << port);
  }

  freeaddrinfo(head);

  return ret;
}

template < class P >
int connect(socket_stream<P> &stream, peer_base<P> endpoint)
{
  auto pt = endpoint.protocol();

  int fd = ::socket(pt.family(), pt.type(), pt.protocol());

  if (fd < 0) {
    return fd;
  }

  int ret = ::connect(fd, endpoint.data(), endpoint.size());
  if (ret == 0) {
    stream.assign(fd);
  } else {
    os::shutdown(fd, os::shutdown_type::READ_WRITE);
  }

  return ret;
}

}