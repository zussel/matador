#ifndef MATADOR_SOCKET_ACCEPTOR_HPP
#define MATADOR_SOCKET_ACCEPTOR_HPP

#include "matador/net/socket.hpp"

namespace matador {

template < class P >
class socket_acceptor : public socket_base<P>
{
public:
  typedef socket_base<P> base;
  typedef socket_stream<P> stream_type;
  typedef typename base::protocol_type protocol_type;
  typedef typename base::peer_type peer_type;

  socket_acceptor() = default;

  explicit socket_acceptor(peer_type &peer);

  socket_acceptor(const char* hostname, unsigned short port);

  int bind(const char* hostname, unsigned short port);

  int bind(peer_type &peer);

  int listen(int backlog);

  void* get_in_addr(struct sockaddr *sa);

  unsigned short get_port(struct sockaddr *sa);

  std::string get_remote_address(struct sockaddr_storage &remote_addr);

  int accept(stream_type &stream);
  int accept(stream_type &stream, peer_type &endpoint);

  int reuse_address(bool reuse);

  int reuse_address() const;
};

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

#ifdef _WIN32
  const char on = 1;
#else
  const int on = 1;
#endif
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
    detail::throw_logic_error_with_errno("setsockopt error: %s", errno);
  }

  int ret = ::bind(listenfd, peer.data(), peer.size());
  if (ret == 0) {
    // success
    this->assign(listenfd);
  } else {
    detail::throw_logic_error_with_errno("couldn't bind fd: %s", errno);
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

  os::inet_ntop(remote_addr.ss_family,
                get_in_addr((struct sockaddr *)&remote_addr),
                s, sizeof s);

  std::stringstream ra;

  ra << s << ":" << get_port((struct sockaddr *)&remote_addr);
  return ra.str();
}

template < class P >
int socket_acceptor<P>::accept(stream_type &stream)
{
  struct sockaddr_storage remote_addr = {};
//    address_type remote_addr;
  socklen_t addrlen = sizeof(remote_addr);
  int fd =  ::accept(this->id(), (struct sockaddr *)&remote_addr, &addrlen);

  if (fd > 0) {
    stream.assign(fd);
    stream.non_blocking(true);
    stream.cloexec(true);
  } else {
    detail::throw_logic_error_with_errno("accept failed: ", errno);
  }

  return fd;
  //printf("server: got connection from %s\n", get_remote_address(remote_addr).c_str());
}

template<class P>
int socket_acceptor<P>::accept(stream_type &stream, peer_type &endpoint)
{
  socklen_t addrlen = endpoint.size();
  int fd = ::accept(this->id(), endpoint.data(), &addrlen);

  if (fd > 0) {
    stream.assign(fd);
    stream.non_blocking(true);
    stream.cloexec(true);
  } else {
    detail::throw_logic_error_with_errno("accept failed: ", errno);
  }

  return fd;
}

template < class P >
int socket_acceptor<P>::reuse_address(bool reuse)
{
  const int option(reuse ? 1 : 0);
  return setsockopt(this->id(), SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
}

template < class P >
int socket_acceptor<P>::reuse_address() const
{
  size_t option {};
  socklen_t i;
  i = sizeof(option);
  getsockopt(this->id(), SOL_SOCKET, SO_REUSEADDR, (char*)&option, &i);
  return option;
}

}

#endif //MATADOR_SOCKET_ACCEPTOR_HPP
