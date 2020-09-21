#ifndef MATADOR_ADDRESSTEST_HPP
#define MATADOR_ADDRESSTEST_HPP

#include "matador/unit/unit_test.hpp"

class AddressTest : public matador::unit_test
{
public:
  AddressTest();

  void test_address_v4();
  void test_address_v6();
  void test_peer_v4();

  void test_ip();
};


#endif //MATADOR_ADDRESSTEST_HPP
