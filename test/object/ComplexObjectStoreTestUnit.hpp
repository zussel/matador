#ifndef COMPLEX_OBJECTSTORE_TEST_UNIT
#define COMPLEX_OBJECTSTORE_TEST_UNIT

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

class ComplexObjectStoreTestUnit : public oos::unit_test
{
public:
  ComplexObjectStoreTestUnit();
  virtual ~ComplexObjectStoreTestUnit() {}
  
  virtual void initialize();
  virtual void finalize();
  
  void first();
  void item_list();
  void album();
  void serializer();
  void ref_ptr_counter();
  void delete_object();

private:
  oos::object_store ostore_;
};

#endif /* COMPLEX_OBJECTSTORE_TEST_UNIT */
