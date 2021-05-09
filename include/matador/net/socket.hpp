#ifndef MATADOR_SOCKET_HPP
#define MATADOR_SOCKET_HPP

#include <stdexcept>
#include <sstream>

#ifdef _WIN32
#else
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <cstring>

namespace matador {

/**
 * Base class for several kind of socket
 * classes (acceptor, stream) representing a
 * socket. The protocol is selected via the
 * template parameter (/sa tcp and udp classes)
 *
 * @tparam P Protocol type
 */
template < class P >
class socket_base
{
public:
  typedef P protocol_type;            /**< Shortcut to the protocol type */
  typedef typename P::peer peer_type; /**< Shortcut to the peer type */

  /**
   * Creates a socket for a specific given
   * protocol
   *
   * @param protocol Initial protocol
   */
  explicit socket_base(const protocol_type &protocol);

  /**
   * Creates a socket with the given peer
   *
   * @param peer Peer used to initialze the socket
   */
  explicit socket_base(const peer_type &peer);

  /**
   * Opens a socket. On success a positive socket id (fd)
   * is returned. In case of error -1 is returned and
   * errno is set.
   *
   * @param protocol Protocol for which a socket is created
   * @return The socket fd or -1 on error
   */
  int open(const protocol_type &protocol);

  /**
   * Closes the open socket
   */
  void close();

  /**
   * Returns true if the socket is open (created)
   *
   * @return True on open socket
   */
  bool is_open() const;

  /**
   * Releases the socket fd and sets
   * the internal socket to zero (0).
   *
   * After the socket is released the user
   * is in charge to take of the socket
   *
   * @return The released socket fd
   */
  int release();

  /**
   * Connect to the given peer. If the connection
   * could be established true is returned,
   * otherwise false is returned and errno is set.
   *
   * @param p Peer to execute to
   * @return True on successful connection
   */
  bool connect(const typename protocol_type::peer &p);

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

  /**
   * Set or unset the cose on exec flag
   * for the socket
   *
   * @param nb Flag to set or unset cloexec option
   */
  void cloexec(bool nb);

  /**
   * Returns true if close on exec option is set
   *
   * @return True on set cloexec option
   */
  bool cloexec() const;

  /**
   * Sets a socket option represented by name. If option
   * was successfully set true is returned. Otherwise false
   * and errno ist set.
   *
   * @param name Option name
   * @param value Flag to set or unset the option
   * @return True on success
   */
  bool options(int name, bool value);

  /**
   * Returns the underlying socket fd
   *
   * @return Underlying socket fd
   */
  int id() const;

  /**
   * Assigns the given socket fd to this
   * socket. If the socket is already opened
   * an exception is thrown.
   *
   * @param sock The socket fd to assign
   */
  void assign(int sock);

protected:
/// @cond MATADOR_DEV
  socket_base() = default;
  ~socket_base() = default;

  int open(int family, int type, int protocol);

  int sock_ = 0;
  std::string name_;
#ifdef _WIN32
  bool is_nonblocking_ = false;
#endif
/// @endcond
};

}

#include "matador/net/socket.tpp"

#endif //MATADOR_SOCKET_HPP
