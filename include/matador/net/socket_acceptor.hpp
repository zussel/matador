#ifndef MATADOR_SOCKET_ACCEPTOR_HPP
#define MATADOR_SOCKET_ACCEPTOR_HPP

#include "matador/net/socket.hpp"

namespace matador {

/**
 * The socket acceptor class provides the
 * base functionality of the socket base class
 * plus acceptor specific functions like
 * listen, bind or accept
 *
 * @tparam P Socket protocol type
 */
template < class P >
class socket_acceptor : public socket_base<P>
{
public:
  typedef socket_base<P> base;                        /**< Shortcut to base socket type */
  typedef socket_stream<P> stream_type;               /**< Shortcut to socket stream type */
  typedef typename base::protocol_type protocol_type; /**< Shortcut to protocol type */
  typedef typename base::peer_type peer_type;         /**< Shortcut to peer type */

  /**
   * Default constructor
   */
  socket_acceptor() = default;

  /**
   * Constructs a socket acceptor for
   * the given peer
   *
   * @param peer Peer to construct an acceptor  from
   */
  explicit socket_acceptor(peer_type &peer);

  /**
   * Constructs an acceptor for given hostname and port
   *
   * @param hostname Hostname of the accepting endpoint
   * @param port Portnumber of the accepting endpoint
   */
  socket_acceptor(const char* hostname, unsigned short port);

  /**
   * Creates a listening socket and binds
   * the given hostname and port to it
   *
   * Returns zero (0) on success and -1 on error
   * with errno set
   *
   * @param hostname Hostname to bind
   * @param port Portnumber to bind
   * @return Returns zero (0) on success.
   */
  int bind(const char* hostname, unsigned short port);

  /**
   * Creates a listening socket and binds
   * the given peer endpoint to it
   *
   * Returns zero (0) on success and -1 on error
   * with errno set
   *
   * @param peer Binds the given peer endpoint to the socket
   * @return Returns zero (0) on success.
   */
  int bind(peer_type &peer);

  /**
   * Start listening to the bound endpoint using
   * the internally created socket.
   *
   * Returns zero (0) on success and -1 on error
   * with errno set
   *
   * @param backlog Number of backlog
   * @return Returns zero (0) on success.
   */
  int listen(int backlog);

  /**
   * Returns a pointer to the underlying
   * concrete internet address of the given
   * socket address structure
   *
   * @param sa Socket address
   * @return Pointer to the internet address
   */
  void* get_in_addr(struct sockaddr *sa);

  /**
   * Returns the port number of the given
   * socket address structure
   *
   * @param sa Socket address
   * @return The port number
   */
  unsigned short get_port(struct sockaddr *sa);

  /**
   * Get the remote address and port as string
   * representation.
   *
   * @param remote_addr Remote socket address structure
   * @return String representation of the remote address
   */
  std::string get_remote_address(struct sockaddr_storage &remote_addr);

  /**
   * Accept a connection and assign the socket descriptor
   * to the given socket stream.
   *
   * Once the descriptor is assigned to the stream it
   * can be used to read and write data to it.
   *
   * @param stream Stream to use after connection was accepted
   * @return The fd of the new connection
   */
  int accept(stream_type &stream);

  /**
   * Accept a connection and assign the socket descriptor
   * to the given socket stream.
   *
   * Once the descriptor is assigned to the stream it
   * can be used to read and write data to it.
   *
   * The given peer endpoint is filled with the
   * address information of the remote endpoint.
   *
   * @param stream Stream to use after connection was accepted
   * @param endpoint Will be filled with the remote endpoint information
   * @return The fd of the new connection
   */
  int accept(stream_type &stream, peer_type &endpoint);

  /**
   * Sets or clears the reuse address flag. If the
   * given value is true the flag is set otherwise the
   * flag is cleared.
   *
   * @param reuse Indicates if the reuse address flag should be set
   * @return 0 if setting was successful, -1 on error
   */
  int reuse_address(bool reuse);

  /**
   * Returns true if the flag is set otherwise
   * false is returned.
   *
   * @return True if flag is set
   */
  bool reuse_address() const;
};

/// @cond MATADOR_DEV
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
//  } else {
//    detail::throw_logic_error_with_errno("accept failed: %s", errno);
  }

  return fd;
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
//  } else {
//    detail::throw_logic_error_with_errno("accept failed: %s", errno);
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
bool socket_acceptor<P>::reuse_address() const
{
  size_t option {};
  socklen_t i;
  i = sizeof(option);
  getsockopt(this->id(), SOL_SOCKET, SO_REUSEADDR, (char*)&option, &i);
  return option > 0;
}

/// @endcond

}

#endif //MATADOR_SOCKET_ACCEPTOR_HPP
