#ifndef MATADOR_REACTORTEST_HPP
#define MATADOR_REACTORTEST_HPP

#include "matador/unit/unit_test.hpp"

#include <thread>

class ReactorTest : public matador::unit_test
{
public:
  ReactorTest();
  ~ReactorTest() override;

  void finalize() override;

  void test_event_types();
  void test_fdset();
  void test_shutdown();
  void test_send_receive();
  void test_timeout();

private:
  std::thread worker_thread_;
};


#endif //MATADOR_REACTORTEST_HPP
