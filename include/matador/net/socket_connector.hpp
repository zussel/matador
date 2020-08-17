#ifndef MATADOR_SOCKET_CONNECTOR_HPP
#define MATADOR_SOCKET_CONNECTOR_HPP

#include "matador/net/socket.hpp"

namespace matador {

template < class P >
class socket_connector : public socket_base<P>
{
public:
  typedef socket_base<P> base;
  typedef typename base::protocol_type protocol_type;
  typedef typename base::peer_type peer_type;

  socket_connector() = default;

  socket_connector(const char* hostname, unsigned short port);

  int connect(const char* hostname, unsigned short port);
};

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
//        throw_logic_error("couldn't connect: " << strerror(errno));
    }

    // bind error, close and try next one
    os::shutdown(connfd, os::shutdown_type::READ_WRITE);
  } while ( (res = res->ai_next) != nullptr);

  if (res == nullptr) {
    throw_logic_error("couldn't connect to " << hostname << ":" << port);
  }

  freeaddrinfo(head);

  return ret;
}
}

#endif //MATADOR_SOCKET_CONNECTOR_HPP
