#include "matador/net/socket.hpp"

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
  ::shutdown(sock_, SHUT_RDWR);
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
  unsigned long nonblock = 1;
    switch(flags) {
    case O_NONBLOCK:
      // fcntl doesn't do the right thing, but the simular ioctl does
      // warning: is that still true? and does it the right thing for
      // set blocking as well?
      return ioctlsocket(fd, FIONBIO, &nonblock);
    default:
      return 0;
    }
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
  int val = fcntl(sock_, F_GETFL, 0);
  if (val < 0) {
    std::string err(strerror(errno));
    throw std::logic_error("fcntl: couldn't get flags (" + err + ")");
  }
  return (val & O_NONBLOCK) > 0;
}

template < class P >
void socket_base<P>::cloexec(bool nb)
{
#ifdef WIN32
  unsigned long cloexec = 1;
    switch(flags) {
    case FD_CLOEXEC:
      // fcntl doesn't do the right thing, but the simular ioctl does
      // warning: is that still true? and does it the right thing for
      // set blocking as well?
      return ioctlsocket(fd, FIONBIO, &cloexec);
    default:
      return 0;
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
  if (getpeername(sock, (struct sockaddr *)&addr, &addr_size) == -1) {
    //throw std::logic_error(strerror(errno));
  }
  char *clientip = new char[20];
  char s[INET6_ADDRSTRLEN];
  inet_ntop(addr.sin_family, &addr.sin_addr, s, sizeof s);
  strcpy(clientip, inet_ntoa(addr.sin_addr));

  sock_ = sock;
}

template < class P >
int socket_base<P>::open(int family, int type, int protocol)
{
  sock_ = ::socket(family, type, protocol);
  return sock_;
}

/*
 * socket_stream
 */
template < class P >
socket_stream<P>::socket_stream(const protocol_type &protocol)
: base(protocol)
{}

template < class P >
template < class Buffer >
long socket_stream<P>::receive(Buffer &buffer)
{
  return ::recv(this->id(), buffer.data(), buffer.capacity(), 0);
}

template < class P >
template < class Buffer >
long socket_stream<P>::send(const Buffer &buffer)
{
  return ::send(this->id(), buffer.data(), buffer.size(), 0);
}

template < class P >
socket_acceptor<P>::socket_acceptor(peer_type &peer)
  : socket_base<P>(peer)
{
  bind(peer);
}

template < class P >
socket_acceptor<P>::socket_acceptor(const char* hostname, unsigned short port)
{
  bind(hostname, port);
}

template < class P >
int socket_acceptor<P>::bind(const char* hostname, unsigned short port)
{
  char portstr[6];
  sprintf(portstr, "%d", port);
//    const char* portname = "daytime";
  struct addrinfo hints = {};
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  struct addrinfo* res = nullptr;
  struct addrinfo* head = nullptr;
  int err = getaddrinfo(hostname, portstr, &hints, &res);
  if (err != 0) {
    throw_logic_error("failed to resolve local socket address (error: " << err << ")");
  }

  head = res;

  int listenfd = 0;
  int ret = 0;
  const int on = 1;
  do {
    listenfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenfd < 0) {
      // error, try next one
      continue;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
      throw std::logic_error(strerror(errno));
    }

    ret = ::bind(listenfd, res->ai_addr, res->ai_addrlen);
    if (ret == 0) {
      // success
      this->assign(listenfd);
      break;
    } else {
      throw_logic_error("couldn't bind to " << hostname << ":" << port << ": " << strerror(errno));
    }

    // bind error, close and try next one
    this->close();
  } while ( (res = res->ai_next) != nullptr);

  if (res == nullptr) {
    throw_logic_error("couldn't bind to " << hostname << ":" << port);
  }

  freeaddrinfo(head);

  return ret;
}

template < class P >
int socket_acceptor<P>::bind(peer_type &peer)
{
  int listenfd = ::socket(peer.protocol().family(), peer.protocol().type(), peer.protocol().protocol());
  if (listenfd < 0) {
    // error, try next one
    return listenfd;
  }

  const int on = 1;
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
    throw std::logic_error(strerror(errno));
  }

  int ret = ::bind(listenfd, peer.data(), peer.size());
  if (ret == 0) {
    // success
    this->assign(listenfd);
  } else {
    throw_logic_error("couldn't bind: " << strerror(errno));
  }
  size_t s = peer.size();
  ret = getsockname(this->id(), peer.data(), (socklen_t*)&s);
  return ret;
}

template < class P >
int socket_acceptor<P>::listen(int backlog)
{
  return ::listen(this->id(), backlog);
}

template < class P >
void* socket_acceptor<P>::get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

template < class P >
unsigned short socket_acceptor<P>::get_port(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return ntohs(((struct sockaddr_in*)sa)->sin_port);
  } else {
    return ntohs(((struct sockaddr_in6*)sa)->sin6_port);
  }
}

template < class P >
std::string socket_acceptor<P>::get_remote_address(struct sockaddr_storage &remote_addr)
{
  char s[INET6_ADDRSTRLEN];

  inet_ntop(remote_addr.ss_family,
            get_in_addr((struct sockaddr *)&remote_addr),
            s, sizeof s);

  std::stringstream ra;

  ra << s << ":" << get_port((struct sockaddr *)&remote_addr);
  return ra.str();
}

template < class P >
int socket_acceptor<P>::accept(socket_base<protocol_type> &sock)
{
  struct sockaddr_storage remote_addr = {};
//    address_type remote_addr;
  socklen_t addrlen = sizeof(remote_addr);
  int fd = ::accept(this->id(), (struct sockaddr *)&remote_addr, &addrlen);
  if (fd > 0) {
    sock.assign(fd);
    sock.non_blocking(true);
    sock.cloexec(true);
  }

  printf("server: got connection from %s\n", get_remote_address(remote_addr).c_str());

  return fd;
}

template < class P >
int socket_acceptor<P>::reuse_address(bool reuse)
{
  const int option(reuse ? 1 : 0);
  return setsockopt(this->id(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

template < class P >
int socket_acceptor<P>::reuse_address() const
{
  size_t option {};
  socklen_t i;
  i = sizeof(option);
  getsockopt(this->id(), SOL_SOCKET, SO_REUSEADDR, &option, &i);
  return option;
}

/*
 * socket connector
 */
template < class P >
socket_connector<P>::socket_connector(const char* hostname, unsigned short port)
{
  connect(hostname, port);
}

template < class P >
int socket_connector<P>::connect(const char* hostname, unsigned short port)
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
    throw_logic_error("failed to resolve local socket address (error: " << err << ")");
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
      this->assign(connfd);
      break;
//      } else {
//        throw_logic_error("couldn't connect: " << strerror(errno));
    }

    // bind error, close and try next one
    this->close();
  } while ( (res = res->ai_next) != nullptr);

  if (res == nullptr) {
    throw_logic_error("couldn't connect to " << hostname << ":" << port);
  }

  freeaddrinfo(head);

  return ret;
}

}