#ifndef TIMETESTUNIT_HPP
#define TIMETESTUNIT_HPP

#include "oos/unit/unit_test.hpp"

class TimeTestUnit : public oos::unit_test
{
public:
  TimeTestUnit();

  void test_create();
  void test_initialize();
  void test_invalid();
  void test_copy();
  void test_assign();
  void test_compare();
  void test_modify();
  void test_parse();
  void test_format();
};

#endif /* TIMETESTUNIT_HPP */

