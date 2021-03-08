#include <iostream>

#include "matador/net/os.hpp"

#include "echo_client.hpp"

using namespace matador;

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cout << "usage: echo_client [port]\n";
    return 1;
  } else {

    net::init();

    echo_client client(argv[1]);

    client.run();

    net::cleanup();
  }
}
