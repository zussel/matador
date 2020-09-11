#ifndef MATADOR_IO_SERVICE_HPP
#define MATADOR_IO_SERVICE_HPP

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"

namespace matador {

class io_service
{
public:
  io_service() = default;

  void run();

  template < typename AcceptCallback >
  void on_accept(acceptor &ac, const tcp::peer &ep, AcceptCallback accept_callback)
  {
    ac.accecpt(ep, [](tcp::socket sock, tcp::peer p, acceptor *accptr) {
      return std::shared_ptr<handler>(nullptr);
    });
  }

  template< typename RC >
  void on_read(RC rc)
  {

  }
  void on_write();

private:
  reactor reactor_;
};

}
#endif //MATADOR_IO_SERVICE_HPP
