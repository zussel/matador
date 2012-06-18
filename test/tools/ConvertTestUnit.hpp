#ifndef CONVERTTESTUNIT_HPP
#define CONVERTTESTUNIT_HPP

#include "unit/unit_test.hpp"

class ConvertTestUnit : public oos::unit_test
{
public:
  ConvertTestUnit();
  virtual ~ConvertTestUnit();
  
  void convert_test();

  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}
};

#endif /* CONVERTTESTUNIT_HPP */
