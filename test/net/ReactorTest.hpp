#ifndef MATADOR_REACTORTEST_HPP
#define MATADOR_REACTORTEST_HPP

#include "matador/unit/unit_test.hpp"

class ReactorTest : public matador::unit_test
{
public:
  ReactorTest();

  void test_reactor_timer();
};


#endif //MATADOR_REACTORTEST_HPP
