#ifndef MATADOR_HTTPCLIENTTEST_HPP
#define MATADOR_HTTPCLIENTTEST_HPP

#include "matador/unit/unit_test.hpp"

class HttpClientTest : public matador::unit_test
{
public:
  HttpClientTest();

  void test_get();
  void test_post();
  void test_put();
  void test_delete();
};


#endif //MATADOR_HTTPCLIENTTEST_HPP
