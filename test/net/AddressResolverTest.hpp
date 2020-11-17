#include "matador/unit/unit_test.hpp"

#ifndef MATADOR_ADDRESSRESOLVERTEST_HPP
#define MATADOR_ADDRESSRESOLVERTEST_HPP


class AddressResolverTest : public matador::unit_test
{
public:
  AddressResolverTest();

  void test_resolver_v4();
};


#endif //MATADOR_ADDRESSRESOLVERTEST_HPP
