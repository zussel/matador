#ifndef MATADOR_HTTP_SERVER_HPP
#define MATADOR_HTTP_SERVER_HPP

#include "matador/net/acceptor.hpp"
#include "matador/net/io_service.hpp"

namespace matador {
namespace http {

class server
{
public:
  explicit server(unsigned short port);

  void run();

private:
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;

};
}

}

#endif //MATADOR_HTTP_SERVER_HPP
