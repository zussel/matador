#ifndef MATADOR_IO_SERVICE_HPP
#define MATADOR_IO_SERVICE_HPP

#include "matador/utils/buffer.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/stream_handler.hpp"

namespace matador {

class io_service
{
public:
  io_service();

  void run();

  template < typename AcceptCallback >
  void on_accept(const std::shared_ptr<acceptor>& ac, const tcp::peer &ep, AcceptCallback accept_callback);

private:
  logger log_;
  reactor reactor_;
};

template<typename AcceptCallback>
void io_service::on_accept(const std::shared_ptr<acceptor>& ac, const tcp::peer &ep, AcceptCallback accept_callback)
{
  log_.info("registering acceptor for %s", ep.to_string().c_str());
  ac->accecpt(ep, [accept_callback](tcp::socket sock, tcp::peer p, acceptor *accptr) {
    return std::make_shared<stream_handler>(sock, p, accptr, accept_callback);
  });

  reactor_.register_handler(ac, event_type::ACCEPT_MASK);
}

}
#endif //MATADOR_IO_SERVICE_HPP
