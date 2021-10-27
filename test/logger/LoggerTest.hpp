#ifndef MATADOR_LOGGERTEST_HPP
#define MATADOR_LOGGERTEST_HPP

#include "matador/unit/unit_test.hpp"

class LoggerTest : public matador::unit_test
{
public:
  LoggerTest();

  void test_log_level_range();
  void test_file_sink();
  void test_rotating_file_sink();
  void test_logger();
  void test_logging();
  void test_stdout();
  void test_stderr();
  void test_log_level();

private:
  void validate_log_file_line(const std::string &filename, int line_index, const std::string &level, const std::string &src, const std::string &msg);
};


#endif //MATADOR_LOGGERTEST_HPP
