#ifndef MATADOR_SOCKET_HPP
#define MATADOR_SOCKET_HPP

#include <stdexcept>
#include <sstream>

#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

namespace matador {

template < class P >
class socket_base
{
public:
  typedef P protocol_type;
  typedef typename P::peer peer_type;

  explicit socket_base(const protocol_type &protocol);
  explicit socket_base(const peer_type &peer);

  int open(const protocol_type &protocol);

  void close();

  bool is_open() const;

  int release();

  int connect(const typename protocol_type::peer &p);

  void non_blocking(bool nb);

  bool non_blocking() const;

  void cloexec(bool nb);

  bool cloexec() const;

  int options(int name, bool value);

  int id() const;

  void assign(int sock);

protected:
  socket_base() = default;
  ~socket_base() = default;

  int open(int family, int type, int protocol);

  int sock_ = 0;
  std::string name_;
};

/*
 * socket stream
 */
template < class P >
class socket_stream : public socket_base<P>
{
public:
  typedef socket_base<P> base;
  typedef typename base::protocol_type protocol_type;
  typedef typename base::peer_type peer_type;

  socket_stream() = default;

  explicit socket_stream(const protocol_type &protocol);

  template < class Buffer >
  ssize_t receive(Buffer &buffer);

  template < class Buffer >
  ssize_t send(const Buffer &buffer);
};

/*
 * socket acceptor
 */
template < class P >
class socket_acceptor : public socket_base<P>
{
public:
  typedef socket_base<P> base;
  typedef typename base::protocol_type protocol_type;
  typedef typename base::peer_type peer_type;
  typedef typename peer_type::address_type address_type;

  socket_acceptor() = default;

  explicit socket_acceptor(peer_type &peer);

  socket_acceptor(const char* hostname, unsigned short port);

  int bind(const char* hostname, unsigned short port);

  int bind(peer_type &peer);

  int listen(int backlog);

  void* get_in_addr(struct sockaddr *sa);

  unsigned short get_port(struct sockaddr *sa);

  std::string get_remote_address(struct sockaddr_storage &remote_addr);

  int accept(socket_base<protocol_type> &sock);

  int reuse_address(bool reuse);

  int reuse_address() const;
};

template < class P >
class socket_connector : public socket_base<P>
{
public:
  typedef socket_base<P> base;
  typedef typename base::protocol_type protocol_type;
  typedef typename base::peer_type peer_type;
  typedef typename peer_type::address_type address_type;

  socket_connector() = default;

  socket_connector(const char* hostname, unsigned short port);

  int connect(const char* hostname, unsigned short port);
};

}

#include "matador/net/socket.tpp"

#endif //MATADOR_SOCKET_HPP
