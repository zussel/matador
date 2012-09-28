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

  /**
   * convert from one data type to another
   * with the following data types
   *
   *  1. char
   *  2. bool
   *  3. short
   *  4. int
   *  5. long
   *  6. unsigned short
   *  7. unsigned int
   *  8. unsigned long
   *  9. float
   * 10. double
   * 11. const char*
   * 12. std::string
   * 13. oos::varchar_base
   */
  void convert_to_char();
  void convert_to_bool();
  void convert_to_short();
  void convert_to_int();
  void convert_to_long();
  void convert_to_unsigned_char();
  void convert_to_unsigned_short();
  void convert_to_unsigned_int();
  void convert_to_unsigned_long();
  void convert_to_float();
  void convert_to_double();
  void convert_to_char_pointer();
  void convert_to_string();
  void convert_to_varchar();

private:
  /*
  template < class T, class U >
  void convert_expect_success(const T& in, U &out)
  {
  }
  template < class T, class U >
  void convert_expect_success(const T& in, U &out, int size)
  {
  }
  template < class T, class U >
  void convert_expect_success(const T& in, U &out, int size, int precision)
  {
  }
  template < class T, class U >
  void convert_expect_failure(const T& in, U &out)
  {
  }
  template < class T, class U >
  void convert_expect_failure(const T& in, U &out, int size)
  {
  }
  template < class T, class U >
  void convert_expect_failure(const T& in, U &out, int size, int precision)
  {
  }
  */
};

#endif /* CONVERTTESTUNIT_HPP */
