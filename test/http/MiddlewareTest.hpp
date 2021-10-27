#ifndef MATADOR_MIDDLEWARETEST_HPP
#define MATADOR_MIDDLEWARETEST_HPP

#include "matador/unit/unit_test.hpp"

class MiddlewareTest : public matador::unit_test
{
public:
  MiddlewareTest();

  void test_middleware();
};


#endif //MATADOR_MIDDLEWARETEST_HPP
