#ifndef DATETESTUNIT_HPP
#define DATETESTUNIT_HPP

#include "unit/unit_test.hpp"

class DateTestUnit : public oos::unit_test
{
public:
  DateTestUnit();
  virtual ~DateTestUnit();

  virtual void initialize() {}
  virtual void finalize() {}

  void test_create();
  void test_julian_date();
  void test_initialize();
};

#endif /* DATETESTUNIT_HPP */

