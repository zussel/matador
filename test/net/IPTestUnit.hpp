#ifndef MATADOR_IPTESTUNIT_HPP
#define MATADOR_IPTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class IPTestUnit : public matador::unit_test
{
public:
  IPTestUnit();

  void test_tcp();
  void test_udp();
};

#endif /* MATADOR_IPTESTUNIT_HPP */