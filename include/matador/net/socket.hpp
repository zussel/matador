#ifndef MATADOR_SOCKET_HPP
#define MATADOR_SOCKET_HPP

#include <stdexcept>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

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

  /**
   * Sets the socket either blocking (false) or
   * non blocking (true).
   *
   * @param nb True sets the socket non blocking false blocking
   */
  void non_blocking(bool nb);

  /**
   * Returns true if the socket is non blocking
   * otherwise returns false
   * @return True if socket is non blocking
   */
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
#ifdef _WIN32
  bool is_nonblocking_ = false;
#endif
};

}

#include "matador/net/socket.tpp"

#endif //MATADOR_SOCKET_HPP
