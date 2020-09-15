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
  void on_accept(acceptor &ac, const tcp::peer &ep, AcceptCallback accept_callback);

  template< typename ReadCallback >
  void on_read(ReadCallback read_callback);

  template< typename WriteCallback >
  void on_write(WriteCallback write_callback);

private:
  reactor reactor_;
};

template<typename AcceptCallback>
void io_service::on_accept(acceptor &ac, const tcp::peer &ep, AcceptCallback accept_callback)
{
  ac.accecpt(ep, [](tcp::socket sock, tcp::peer p, acceptor *accptr) {
    return std::shared_ptr<handler>(nullptr);
  });
}

template<typename ReadCallback>
void io_service::on_read(ReadCallback read_callback)
{

}

template<typename WriteCallback>
void io_service::on_write(WriteCallback write_callback)
{

}

}
#endif //MATADOR_IO_SERVICE_HPP
