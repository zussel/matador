#ifndef MATADOR_LOGGERTEST_HPP
#define MATADOR_LOGGERTEST_HPP

#include "matador/unit/unit_test.hpp"

class LoggerTest : public matador::unit_test
{
public:
  LoggerTest();

  void test_file_sink();
  void test_logger();
  void test_logging();
  void test_stdout();
  void test_stderr();

private:
  void validate_log_file_line(const std::string &filename, int line_index, const std::string &level, const std::string &src, const std::string &msg);
};


#endif //MATADOR_LOGGERTEST_HPP
