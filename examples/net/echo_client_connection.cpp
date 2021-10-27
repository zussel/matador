#include "matador/logger/log_manager.hpp"
#include "matador/utils/buffer_view.hpp"

#include "echo_client_connection.hpp"

#include <iostream>

using namespace matador;

echo_client_connection::echo_client_connection(matador::io_stream &stream, matador::tcp::peer endpoint)
: log_(create_logger("EchoClientConnection"))
, stream_(stream)
, endpoint_(std::move(endpoint))
{}

void echo_client_connection::start()
{
  write();
}

void echo_client_connection::read()
{
  auto self(shared_from_this());
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int nread) {
    if (ec == 0) {
      std::cout << "Answer (size " << nread << "): " << std::string(buf_.data(), buf_.size()) << "\n";
      write();
    }
  });
}

void echo_client_connection::write()
{
  std::cout << "Message: ";
  std::string message;
  std::cin >> message;
  if (message == "exit") {
    stream_.close_stream();
    return;
  }
  std::list<matador::buffer_view> data{ matador::buffer_view(message) };

  auto self(shared_from_this());
  stream_.write(data, [this, self](int ec, int nwrite) {
    if (ec == 0) {
      log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
      read();
    }
  });
}
