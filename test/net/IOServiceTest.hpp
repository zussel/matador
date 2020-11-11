#ifndef MATADOR_IOSERVICETEST_HPP
#define MATADOR_IOSERVICETEST_HPP

#include "matador/unit/unit_test.hpp"

#include <thread>

class IOServiceTest : public matador::unit_test
{
public:
  IOServiceTest();

  void test_shutdown();
  void test_send_receive();
};


#endif //MATADOR_IOSERVICETEST_HPP
