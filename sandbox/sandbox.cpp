#include <matador/utils/buffer.hpp>
#include <utility>
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

  tcp::peer endpoint(address::v4::any() , 7090);

  auto acceptor_7090 = std::make_shared<acceptor>(endpoint, [](const tcp::socket& sock, acceptor *accptr) {
    return std::make_shared<echo_handler>(sock, accptr);
  });

  reactor rctr;
  rctr.register_handler(acceptor_7090, event_type::ACCEPT_MASK);

  rctr.run();



//  http::server serv;
//
//  serv.on_get("/", [](http::request &request) {
//    return http::response;
//  });
//
//  serv.listen(7090);
}

echo_handler::echo_handler(tcp::socket sock, acceptor *accptr)
  : stream_(std::move(sock)), acceptor_(accptr)
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
    /*
    GET / HTTP/1.1
    Host: localhost:7090
    User-Agent: curl/7.70.0
    Accept: * / *
    */
  char buf[16384];
  buffer chunk(buf, 16384);
  auto len = stream_.receive(chunk);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    log_.error("fd %d: error on read: %s", handle(), strerror(errno));
    on_close();
  } else {
    log_.info("received %d bytes", len);
    data_.assign(buf, len);
    log_.info("received data: %s", data_.c_str());
    log_.info("end of data");
  }
}

void echo_handler::on_output()
{
  std::string ret = R"(HTTP/1.1 200 OK
Server: Matador/0.7.0
Content-Length: 111
Content-Language: de
Connection: close
Content-Type: text/html

<!doctype html>
<html>
  <head>
    <title>Dummy!</title>
  </head>
  <body>
    <p>Help!</p>
  </body>
</html>
)";

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
  log_.info("fd %d: closing connection", handle());
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void echo_handler::close()
{
    log_.info("fd %d: closing connection", handle());
    stream_.close();
}

bool echo_handler::is_ready_write() const
{
  return !data_.empty();
}

bool echo_handler::is_ready_read() const
{
  return data_.empty();
}
