#ifndef OOS_OSTEST_HPP
#define OOS_OSTEST_HPP

#include "matador/unit/unit_test.hpp"

class OSTest : public matador::unit_test
{
public:
  OSTest();

  void test_mkdir();
  void test_mkpath();
  void test_remove_file();
  void test_rename_file();
  void test_access_file();
};

#endif // OOS_OSTEST_HPP