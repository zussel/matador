#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"
#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

using namespace matador;

class echo_handler : public handler
{
public:
  void open() override
  {

  }

  int handle() const override
  {
    return 0;
  }

  void on_input() override
  {

  }

  void on_output() override
  {

  }

  void on_except() override
  {

  }

  void on_timeout() override
  {

  }

  void on_close() override
  {

  }

  void close() override
  {

  }

  bool is_ready_write() const override
  {
    return false;
  }

  bool is_ready_read() const override
  {
    return false;
  }

private:
  tcp::socket stream_;
};

int main()
{
  tcp::peer endpoint(tcp::v4(), 7090);

  auto acceptor_7090 = std::make_shared<acceptor>([]() {return new echo_handler;});

  acceptor_7090->open();

  reactor rctr;
  rctr.register_handler(acceptor_7090, event_type::ACCEPT_MASK);

  rctr.run();
}
