#include <iostream>

#include <matador/net/os.hpp>
#include "echo_server.hpp"

using namespace matador;

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cout << "usage: echo_server [port]\n";
    return 1;
  } else {

    net::init();

    echo_server server(std::atoi(argv[1]));

    server.run();

    net::cleanup();
  }
}
