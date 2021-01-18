#ifndef MATADOR_HTTPSERVERTEST_HPP
#define MATADOR_HTTPSERVERTEST_HPP

#include "matador/unit/unit_test.hpp"

class HttpServerTest : public matador::unit_test
{
public:
  HttpServerTest();

  void test_shutdown();
  void test_get();
  void test_post();
};


#endif //MATADOR_HTTPSERVERTEST_HPP
