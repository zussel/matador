#ifndef JSONTESTUNIT_HPP
#define JSONTESTUNIT_HPP

#include "unit/unit_test.hpp"

class JsonTestUnit : public oos::unit_test
{
public:
  JsonTestUnit();
  ~JsonTestUnit();
  
  void simple_test();
  void invalid_test();
  void null_test();
  void bool_test();
  void string_test();
  void number_test();
  void array_test();
  void create_test();
  void access_test();
  void parser_test();
  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}  
};

#endif /* JSONTESTUNIT_HPP */
