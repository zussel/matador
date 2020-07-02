#ifndef MATADOR_SQLLOGGERTEST_HPP
#define MATADOR_SQLLOGGERTEST_HPP

#include "matador/unit/unit_test.hpp"

class SqlLoggerTest : public matador::unit_test
{
public:
  SqlLoggerTest();

  void test_null_logger();
  void test_sql_logger();
};


#endif //MATADOR_SQLLOGGERTEST_HPP
