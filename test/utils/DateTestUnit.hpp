#ifndef DATETESTUNIT_HPP
#define DATETESTUNIT_HPP

#include "oos/unit/unit_test.hpp"

class DateTestUnit : public oos::unit_test
{
public:
  DateTestUnit();

  void test_create();
  void test_julian_date();
  void test_initialize();
  void test_invalid();
  void test_copy();
  void test_assign();
  void test_compare();
  void test_set();
  void test_modify();
  void test_difference();
  void test_to_string();
};

#endif /* DATETESTUNIT_HPP */

