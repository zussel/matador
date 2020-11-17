#ifndef MATADOR_OPTIONALTEST_HPP
#define MATADOR_OPTIONALTEST_HPP

#include "matador/unit/unit_test.hpp"

class OptionalTest : public matador::unit_test
{
public:
  OptionalTest();

  void test_bad_access();
};

#endif //MATADOR_OPTIONALTEST_HPP
