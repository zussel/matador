#include "matador/net/acceptor.hpp"
#include "matador/net/reactor.hpp"
#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

using namespace matador;

class echo_handler : public handler
{
public:
  echo_handler(tcp::socket sock, acceptor *accptr);

  void open() override;

  int handle() const override;

  void on_input() override;

  void on_output() override;

  void on_except() override;

  void on_timeout() override;

  void on_close() override;

  void close() override;

  bool is_ready_write() const override;

  bool is_ready_read() const override;

private:
  tcp::socket stream_;
  acceptor *acceptor_ = nullptr;
};

int main()
{
  auto s = matador::create_file_sink("log/net.log");
  matador::add_log_sink(s);
  matador::add_log_sink(matador::create_stdout_sink());

  tcp::peer endpoint(tcp::v4(), 7090);

  auto acceptor_7090 = std::make_shared<acceptor>([](tcp::socket sock, acceptor *accptr) {return std::make_shared<echo_handler>(sock, accptr);});

  acceptor_7090->open();

  reactor rctr;
  rctr.register_handler(acceptor_7090, event_type::ACCEPT_MASK);

  rctr.run();
}

echo_handler::echo_handler(tcp::socket sock, acceptor *accptr)
  : stream_(sock), acceptor_(accptr)
{

}

void echo_handler::open()
{

}

int echo_handler::handle() const
{
  return 0;
}

void echo_handler::on_input()
{

}

void echo_handler::on_output()
{

}

void echo_handler::on_except()
{

}

void echo_handler::on_timeout()
{

}

void echo_handler::on_close()
{

}

void echo_handler::close()
{

}

bool echo_handler::is_ready_write() const
{
  return false;
}

bool echo_handler::is_ready_read() const
{
  return false;
}
