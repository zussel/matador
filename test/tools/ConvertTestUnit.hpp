#ifndef CONVERTTESTUNIT_HPP
#define CONVERTTESTUNIT_HPP

#include "unit/unit_test.hpp"

class ConvertTestUnit : public oos::unit_test
{
public:
  ConvertTestUnit();
  virtual ~ConvertTestUnit();
  
  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}

  void convert_test();

};

#endif /* CONVERTTESTUNIT_HPP */
