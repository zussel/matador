#ifndef OBJECTVECTORTESTUNIT_HPP
#define OBJECTVECTORTESTUNIT_HPP

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

class ObjectVectorTestUnit : public oos::unit_test
{
public:
  ObjectVectorTestUnit();
  virtual ~ObjectVectorTestUnit();
  
  virtual void initialize();
  virtual void finalize();

  void test_empty();
  void test_int_vector();
  void test_ref_vector();
  void test_ptr_vector();

private:
  oos::object_store ostore_;
};

#endif /* OBJECTVECTORTESTUNIT_HPP */
