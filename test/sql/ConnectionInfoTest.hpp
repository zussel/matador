#ifndef MATADOR_CONNECTIONINFOTEST_HPP
#define MATADOR_CONNECTIONINFOTEST_HPP

#include "matador/unit/unit_test.hpp"

class ConnectionInfoTest  : public matador::unit_test
{
public:
  ConnectionInfoTest();

  void test_parse_connection_info();
  void test_connection_info_to_string();
  void test_connection_info_from_json();
};


#endif //MATADOR_CONNECTIONINFOTEST_HPP
