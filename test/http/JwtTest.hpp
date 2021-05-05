#ifndef MATADOR_JWTTEST_HPP
#define MATADOR_JWTTEST_HPP

#include "matador/unit/unit_test.hpp"

class JwtTest : public matador::unit_test
{
public:
  JwtTest();

  void test_jwt_token();
};


#endif //MATADOR_JWTTEST_HPP
