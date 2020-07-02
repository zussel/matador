#ifndef TIMETESTUNIT_HPP
#define TIMETESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class TimeTestUnit : public matador::unit_test
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
  void test_to_date();
  void test_strftime();
};

#endif /* TIMETESTUNIT_HPP */

