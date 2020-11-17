#ifndef MATADOR_SOCKETTEST_HPP
#define MATADOR_SOCKETTEST_HPP

#include "matador/unit/unit_test.hpp"

class SocketTest : public matador::unit_test
{
public:
  SocketTest();

  void test_socket_v4();
  void test_acceptor_v4();
};


#endif //MATADOR_SOCKETTEST_HPP
