#ifndef MATADOR_IO_SERVICE_HPP
#define MATADOR_IO_SERVICE_HPP

#include "matador/net/reactor.hpp"

namespace matador {

class io_service
{
public:
  io_service() = default;

  template < typename AC >
  void on_accept(AC ac)
  {

  }
  void on_read();
  void on_write();

private:
  reactor reactor_;
};

}
#endif //MATADOR_IO_SERVICE_HPP
