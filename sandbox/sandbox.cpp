#include "matador/utils/buffer.hpp"
#include "matador/utils/os.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"
#include "matador/net/socket.hpp"
#include "matador/net/ip.hpp"
#include "matador/net/reactor.hpp"
#include "matador/net/address_resolver.hpp"
#include "matador/net/io_service.hpp"
#include "matador/net/io_stream.hpp"

#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

#include <utility>
#include <iostream>

using namespace matador;

class echo_server_connection : std::enable_shared_from_this<echo_server_connection>
{
public:
  explicit echo_server_connection(io_stream &stream, tcp::peer endpoint)
    : stream_(stream)
    , endpoint_(std::move(endpoint))
  {}

  void start() {
    read();
  }

  void read() {
    stream_.read(buf_, [this](int ec, int nread) {
      if (ec == 0) {
        std::cout << endpoint_.to_string() << " read (bytes: " << nread << ")> " << buf_.data() << "\n";
        write();
      }
    });
  }
  void write() {
    stream_.write(buf_, [this](int ec, int nwrite) {
      if (ec == 0) {
        std::cout << endpoint_.to_string() << " sent (bytes: " << nwrite << ")\n";
        read();
      }
    });
  }

private:
  buffer buf_;
  io_stream &stream_;
  tcp::peer endpoint_;
};

class echo_server_handler : public handler
{
public:
  echo_server_handler(tcp::socket sock, const tcp::peer& endpoint, acceptor *accptr);

  void open() override;

  int handle() const override;

  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override;
  void on_close() override;
  void close() override;

  bool is_ready_write() const override;
  bool is_ready_read() const override;

private:
  tcp::socket stream_;
  tcp::peer endpoint_;
  acceptor *acceptor_ = nullptr;

  std::string data_;
  logger log_;
};

class echo_client_handler : public handler
{
public:
  echo_client_handler(tcp::socket sock, const tcp::peer& endpoint, connector *cnnctr);

  void open() override;

  int handle() const override;

  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override {}
  void on_close() override;
  void close() override;

  bool is_ready_write() const override;
  bool is_ready_read() const override;

private:
  tcp::socket stream_;
  tcp::peer endpoint_;
  connector *connector_ = nullptr;

  std::string data_;
  logger log_;

  std::string message_;
};

class echo_server
{
public:
  echo_server(const std::string &host, const std::string &port)
    : acceptor_(std::make_shared<acceptor>())
  {
    matador::add_log_sink(matador::create_file_sink("log/server.log"));
    matador::add_log_sink(matador::create_stdout_sink());

    tcp::resolver r;
    auto endpoints = r.resolve(host, port);

    prepare_accept(endpoints);
  }

  void run() {
    service_.run();
  }

private:
  void prepare_accept(std::vector<peer_base<tcp>> endpoints)
  {

    service_.on_accept(acceptor_, endpoints.front(), [](tcp::peer ep, io_stream &stream) {
      // create echo server connection
      auto conn = std::make_shared<echo_server_connection>(stream, std::move(ep));
      conn->start();
    });
  }
private:
  io_service service_;
  std::shared_ptr<acceptor> acceptor_;
};

void start_client(const std::string &port);
void start_server(const std::string &port);

int main(int argc, char* argv[])
{
  if (argc < 3 || (strcmp("server", argv[1]) != 0 && strcmp("client", argv[1]) != 0)) {
    std::cout << "usage: sandbox [server|client] [port]\n";
    return 1;
  } else {


    std::string type = argv[1];

    if (type == "client") {
      start_client(argv[2]);
    } else {
      start_server(argv[2]);
    }
  }

//  http::server serv;
//
//  serv.on_get("/", [](http::request &request) {
//    return http::response;
//  });
//
//  serv.listen(7090);
}

void start_client(const std::string &port)
{
  net::init();

  matador::add_log_sink(matador::create_file_sink("log/client.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  auto echo_connector = std::make_shared<connector>([](const tcp::socket& sock, const tcp::peer &p, connector *cnnctr) {
    return std::make_shared<echo_client_handler>(sock, p, cnnctr);
  });

  reactor r;

  tcp::resolver resolver;
  auto endpoints = resolver.resolve("localhost", port);
  echo_connector->connect(r, endpoints);

  r.run();

  net::cleanup();
}

void start_server(const std::string &port)
{
  net::init();

//  tcp::peer endpoint(address::v4::any() , port);

  echo_server server("localhost", port);

  server.run();

//  matador::add_log_sink(matador::create_file_sink("log/server.log"));
//  matador::add_log_sink(matador::create_stdout_sink());
//
//
//  auto echo_acceptor = std::make_shared<acceptor>(endpoint, [](const tcp::socket& sock, const tcp::peer &p, acceptor *accptr) {
//    return std::make_shared<echo_server_handler>(sock, p, accptr);
//  });
//
//  reactor r;
//  r.register_handler(echo_acceptor, event_type::ACCEPT_MASK);
//
//  r.run();

  net::cleanup();
}

echo_server_handler::echo_server_handler(tcp::socket sock, const tcp::peer& endpoint, acceptor *accptr)
  : stream_(std::move(sock))
  , endpoint_(endpoint)
  , acceptor_(accptr)
  , log_(create_logger("EchoServerHandler"))
{

}

void echo_server_handler::open()
{

}

int echo_server_handler::handle() const
{
  return stream_.id();
}

void echo_server_handler::on_input()
{
    /*
    GET / HTTP/1.1
    Host: localhost:7090
    User-Agent: curl/7.70.0
    Accept: * / *
    */
  buffer chunk;
  auto len = stream_.receive(chunk);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    log_.error("fd %d: error on read: %s", handle(), strerror(errno));
    on_close();
  } else {
    log_.info("received %d bytes", len);
    data_.assign(chunk.data(), len);
    log_.info("received data: %s", data_.c_str());
    log_.info("end of data");
  }
}

void echo_server_handler::on_output()
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

  buffer chunk;

  chunk.append(data_.c_str(), data_.size());
//  chunk.append(ret.c_str(), ret.size());
  auto len = stream_.send(chunk);
  log_.info("sent %d bytes", len);
  data_.clear();
}

void echo_server_handler::on_timeout()
{
  log_.info("hello from the timeout");
}

void echo_server_handler::on_close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void echo_server_handler::close()
{
    log_.info("fd %d: closing connection", handle());
    stream_.close();
}

bool echo_server_handler::is_ready_write() const
{
  return !data_.empty();
}

bool echo_server_handler::is_ready_read() const
{
  return data_.empty();
}

echo_client_handler::echo_client_handler(tcp::socket sock, const tcp::peer &endpoint, connector *cnnctr)
  : stream_(std::move(sock))
  , endpoint_(endpoint)
  , connector_(cnnctr)
  , log_(create_logger("EchoClientHandler"))
{

}

void echo_client_handler::open()
{

}

int echo_client_handler::handle() const
{
  return stream_.id();
}

void echo_client_handler::on_input()
{
  buffer chunk;
  int ret = stream_.receive(chunk);
  message_.clear();
  message_.assign(chunk.data(), ret);
  chunk.reset();

  std::cout << "Answer: " << message_ << "\n";
  message_.clear();
}

void echo_client_handler::on_output()
{
  std::cout << "Message: ";
  std::cin >> message_;
  if (message_ == "exit") {
    close();
    return;
  }
  buffer chunk;

  chunk.append(message_.c_str(), message_.size());
  size_t len = stream_.send(chunk);
  log_.info("sent %d bytes", len);
  chunk.reset();
}

void echo_client_handler::on_close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void echo_client_handler::close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
}

bool echo_client_handler::is_ready_write() const
{
  return message_.empty();
}

bool echo_client_handler::is_ready_read() const
{
  return !message_.empty();
}
