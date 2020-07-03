#include <matador/utils/buffer.hpp>
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

  std::string data_;
  logger log_;
};

int main()
{
  auto s = matador::create_file_sink("log/net.log");
  matador::add_log_sink(s);
  matador::add_log_sink(matador::create_stdout_sink());

  tcp::peer endpoint(tcp::v4(), 7090);

  auto acceptor_7090 = std::make_shared<acceptor>(endpoint, [](tcp::socket sock, acceptor *accptr) {return std::make_shared<echo_handler>(sock, accptr);});

  reactor rctr;
  rctr.register_handler(acceptor_7090, event_type::ACCEPT_MASK);

  rctr.run();
}

echo_handler::echo_handler(tcp::socket sock, acceptor *accptr)
  : stream_(sock), acceptor_(accptr)
  , log_(create_logger("EchoHandler"))
{

}

void echo_handler::open()
{

}

int echo_handler::handle() const
{
  return stream_.id();
}

void echo_handler::on_input()
{
  char buf[16384];
  buffer chunk(buf, 16384);
  auto len = stream_.receive(chunk);
  log_.info("received %d bytes", len);
  log_.info("received data: %s", buf);
  data_.assign(buf, len);
}

void echo_handler::on_output()
{
  std::string ret("<a>Hallo</a>");
  char buf[16384];
  buffer chunk(buf, 16384);
  chunk.append(ret.c_str(), ret.size());
  auto len = stream_.send(chunk);
  log_.info("sent %d bytes", len);
  data_.clear();
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
  return !data_.empty();
}

bool echo_handler::is_ready_read() const
{
  return data_.empty();
}
