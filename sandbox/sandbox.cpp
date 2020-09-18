#include "matador/utils/buffer.hpp"
#include "matador/utils/os.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"
#include "matador/net/socket.hpp"
#include "matador/net/ip.hpp"
#include "matador/net/address_resolver.hpp"
#include "matador/net/io_service.hpp"
#include "matador/net/io_stream.hpp"

#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

#include <utility>
#include <iostream>

using namespace matador;

class echo_server_connection : public std::enable_shared_from_this<echo_server_connection>
{
public:
  explicit echo_server_connection(io_stream &stream, tcp::peer endpoint)
    : log_(create_logger("EchoServerConnection"))
    , stream_(stream)
    , endpoint_(std::move(endpoint))
  {}

  void start() {
    read();
  }

  void read() {
    auto self(shared_from_this());
    stream_.read(buf_, [this, self](int ec, int nread) {
      if (ec == 0) {
        log_.info("%s read (bytes: %d)> %s", endpoint_.to_string().c_str(), nread, buf_.data());
        write();
      }
    });
  }
  void write() {
    auto self(shared_from_this());
    stream_.write(buf_, [this, self](int ec, int nwrite) {
      if (ec == 0) {
        log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
        buf_.reset();
        read();
      }
    });
  }

private:
  logger log_;
  buffer buf_;
  io_stream &stream_;
  tcp::peer endpoint_;
};

class echo_client_connection : public std::enable_shared_from_this<echo_client_connection>
{
public:
  explicit echo_client_connection(io_stream &stream, tcp::peer endpoint)
    : log_(create_logger("EchoClientConnection"))
    , stream_(stream)
    , endpoint_(std::move(endpoint))
  {}

  void start() {
    write();
  }

  void read() {
    auto self(shared_from_this());
    stream_.read(buf_, [this, self](int ec, int nread) {
      if (ec == 0) {
        std::cout << "Answer (size " << nread << "): " << std::string(buf_.data(), buf_.size()) << "\n";
        buf_.reset();
        write();
      }
    });
  }

  void write() {
    std::cout << "Message: ";
    std::string message;
    std::cin >> message;
    if (message == "exit") {
      stream_.close_stream();
      return;
    }
    buf_.append(message.c_str(), message.size());
    auto self(shared_from_this());
    stream_.write(buf_, [this, self](int ec, int nwrite) {
      if (ec == 0) {
        log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
        buf_.reset();
        read();
      }
    });
  }

private:
  logger log_;
  buffer buf_;
  io_stream &stream_;
  tcp::peer endpoint_;
};

class echo_server
{
public:
  explicit echo_server(unsigned short port)
    : acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
  {
    matador::add_log_sink(matador::create_file_sink("log/server.log"));
    matador::add_log_sink(matador::create_stdout_sink());

    prepare_accept();
  }

  void run() {
    service_.run();
  }

private:
  void prepare_accept()
  {

    service_.accept(acceptor_, [](tcp::peer ep, io_stream &stream) {
      // create echo server connection
      auto conn = std::make_shared<echo_server_connection>(stream, std::move(ep));
      conn->start();
    });
  }
private:
  io_service service_;
  std::shared_ptr<acceptor> acceptor_;
};

class echo_client
{
public:
  explicit echo_client(std::string port)
    : connector_(std::make_shared<connector>())
    , port_(std::move(port))
  {
    matador::add_log_sink(matador::create_file_sink("log/client.log"));
    matador::add_log_sink(matador::create_stdout_sink());

    prepare_connect();
  }

  void run() {
    service_.run();
  }

private:
  void prepare_connect()
  {
    service_.connect(connector_, port_, [](tcp::peer ep, io_stream &stream) {
      // create echo server connection
      auto conn = std::make_shared<echo_client_connection>(stream, std::move(ep));
      conn->start();
    });
  }
private:
  io_service service_;
  std::shared_ptr<connector> connector_;
  std::string port_;
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
}

//  http::server serv;
//
//  serv.on_get("/", [](http::request &request) {
//    return http::response;
//  });
//
//  serv.listen(7090);

void start_client(const std::string &port)
{
  net::init();

  echo_client client(port);

  client.run();

  net::cleanup();
}

void start_server(const std::string &port)
{
  net::init();

  echo_server server(std::atoi(port.c_str()));

  server.run();

  net::cleanup();
}

// server
  /*
  GET / HTTP/1.1
  Host: localhost:7090
  User-Agent: curl/7.70.0
  Accept: * / *
  */

// client
//(HTTP/1.1 200 OK
//Server: Matador/0.7.0
//Content-Length: 111
//Content-Language: de
//Connection: close
//Content-Type: text/html
//
//<!doctype html>
//<html>
//  <head>
//    <title>Dummy!</title>
//  </head>
//  <body>
//    <p>Help!</p>
//  </body>
//</html>
