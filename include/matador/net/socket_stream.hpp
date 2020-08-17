#ifndef MATADOR_SOCKET_STREAM_HPP
#define MATADOR_SOCKET_STREAM_HPP

#include "matador/net/socket.hpp"

namespace matador {

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

}

#endif //MATADOR_SOCKET_STREAM_HPP
