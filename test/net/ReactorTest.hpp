#ifndef MATADOR_REACTORTEST_HPP
#define MATADOR_REACTORTEST_HPP

#include "matador/unit/unit_test.hpp"

#include <thread>

class ReactorTest : public matador::unit_test
{
public:
  ReactorTest();

  void test_event_types();
  void test_fdset();
  void test_handler();
  void test_connector();
  void test_shutdown();
  void test_reactor_acceptor();
  void test_reactor_connector();
  void test_timeout();
};


#endif //MATADOR_REACTORTEST_HPP
