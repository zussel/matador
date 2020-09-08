#ifndef MATADOR_IO_SERVICE_HPP
#define MATADOR_IO_SERVICE_HPP

#include "matador/net/reactor.hpp"

namespace matador {

class io_service
{
public:
  io_service() = default;

  void read();
  void write()
private:
  reactor reactor_;
};

}
#endif //MATADOR_IO_SERVICE_HPP
