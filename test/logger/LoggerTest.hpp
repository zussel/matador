#ifndef MATADOR_LOGGERTEST_HPP
#define MATADOR_LOGGERTEST_HPP

#include "matador/unit/unit_test.hpp"

class LoggerTest : public matador::unit_test
{
public:
  LoggerTest();

  void test_logger();
  void test_logging();
  void test_stdout();
  void test_stderr();
};


#endif //MATADOR_LOGGERTEST_HPP
