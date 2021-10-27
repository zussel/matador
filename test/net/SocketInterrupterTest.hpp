#ifndef MATADOR_SOCKETINTERRUPTERTEST_HPP
#define MATADOR_SOCKETINTERRUPTERTEST_HPP

#include "matador/unit/unit_test.hpp"

class SocketInterrupterTest : public matador::unit_test
{
public:
  SocketInterrupterTest();

  void test_interrupter();
};


#endif //MATADOR_SOCKETINTERRUPTERTEST_HPP
