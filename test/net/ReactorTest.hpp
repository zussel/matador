#ifndef MATADOR_REACTORTEST_HPP
#define MATADOR_REACTORTEST_HPP

#include "matador/unit/unit_test.hpp"

class ReactorTest : public matador::unit_test
{
public:
  ReactorTest();

  void test_event_types();
  void test_shutdown();
  void test_send_receive();
  void test_timeout();
};


#endif //MATADOR_REACTORTEST_HPP
