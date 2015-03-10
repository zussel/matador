#ifndef TIMETESTUNIT_HPP
#define TIMETESTUNIT_HPP

#include "unit/unit_test.hpp"

class TimeTestUnit : public oos::unit_test
{
public:
  TimeTestUnit();
  virtual ~TimeTestUnit();

  virtual void initialize() {}
  virtual void finalize() {}

  void test_create();
  void test_initialize();
};

#endif /* TIMETESTUNIT_HPP */

