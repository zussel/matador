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
  void test_shutdown();
  void test_acceptor();
  void test_connector();
  void test_timeout();
};


#endif //MATADOR_REACTORTEST_HPP
