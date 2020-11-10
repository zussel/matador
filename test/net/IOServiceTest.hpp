#ifndef MATADOR_IOSERVICETEST_HPP
#define MATADOR_IOSERVICETEST_HPP

#include "matador/unit/unit_test.hpp"

#include <thread>

class IOServiceTest : public matador::unit_test
{
public:
  IOServiceTest();

  void finalize() override;

  void test_shutdown();
  void test_send_receive();

private:
  std::thread worker_thread_;
};


#endif //MATADOR_IOSERVICETEST_HPP
