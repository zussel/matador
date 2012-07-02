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

  /**
   * convert from one data type to another
   * with the following data types
   *
   *  1. char
   *  2. short
   *  3. int
   *  4. long
   *  5. unsigned short
   *  6. unsigned int
   *  7. unsigned long
   *  8. float
   *  9. double
   * 10. const char*
   * 11. std::string
   * 12. oos::varchar_base
   */
  void convert_to_char();
};

#endif /* CONVERTTESTUNIT_HPP */
