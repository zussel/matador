#ifndef OBJECT_PROTOTYPE_TESTUNIT_HPP
#define OBJECT_PROTOTYPE_TESTUNIT_HPP

#include "unit/unit_test.hpp"

class ObjectPrototypeTestUnit : public oos::unit_test
{
public:
  ObjectPrototypeTestUnit();
  virtual ~ObjectPrototypeTestUnit();
  
  virtual void initialize();
  virtual void finalize();
  
  void empty_store();
  void one_prototype();
  void prototype_hierachy();
  void prototype_traverse();
  void prototype_relation();
};

#endif /* OBJECT_PROTOTYPE_TESTUNIT_HPP */
