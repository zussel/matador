#ifndef MATADOR_SOCKET_STREAM_HPP
#define MATADOR_SOCKET_STREAM_HPP

#ifdef _WIN64
#define ssize_t __int64
#elif _WIN32
#define ssize_t long
#endif

#include "matador/net/socket.hpp"

namespace matador {

/**
 * The class represents a read write socket. It is
 * independent to the protocol type (UDP or TCP).
 *
 * It provides to methods receive and send to read
 * or write data to an open socket.
 *
 * @tparam P Protocol type of the socket
 */
template < class P >
class socket_stream : public socket_base<P>
{
public:
  typedef socket_base<P> base;                        /**< Shortcut to socket base class */
  typedef typename base::protocol_type protocol_type; /**< Shortcut to protocol type */
  typedef typename base::peer_type peer_type;         /**< Shortcut to the peer type */

  /**
   * Creates an uninitialized socket stream
   */
  socket_stream() = default;

  /**
   * Creates a socket stream for the given
   * protocol type.
   *
   * @param protocol Type of the protocol
   */
  explicit socket_stream(const protocol_type &protocol);

  /**
   * Receives data from the underlying open socket.
   * All received data is stored in the given buffer.
   * The limit of the data to receive is the size of
   * the buffer
   *
   * Number of bytes is returned. If returned value is zero (0)
   * the socket was closed. If the return value is -1 an error
   * occurred or the socket was blocked (in case of nonblocking)
   * The concrete error code and message can be retrieved with
   * errno.
   *
   * @tparam Buffer type of the buffer object.
   * @param buffer The buffer object.
   * @return The number of bytes received, -1 on error or 0 on close
   */
  template < class Buffer >
  ssize_t receive(Buffer &buffer);

  /**
   *
   * @tparam Buffer type of the buffer object.
   * @param buffer The buffer object.
   * @return The number of bytes sent, -1 on error or 0 on close
   */
  template < class Buffer >
  ssize_t send(const Buffer &buffer);
};

/// @cond MATADOR_DEV
template < class P >
socket_stream<P>::socket_stream(const protocol_type &protocol)
: base(protocol)
{}

template < class P >
template < class Buffer >
ssize_t socket_stream<P>::receive(Buffer &buffer)
{
  return ::recv(this->id(), buffer.data(), buffer.capacity(), 0);
}

template < class P >
template < class Buffer >
ssize_t socket_stream<P>::send(const Buffer &buffer)
{
  return ::send(this->id(), buffer.data(), buffer.size(), 0);
}
/// @endcond
}

#endif //MATADOR_SOCKET_STREAM_HPP
