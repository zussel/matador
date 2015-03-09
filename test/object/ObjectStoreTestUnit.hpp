#ifndef OBJECTSTORE_TESTUNIT_HPP
#define OBJECTSTORE_TESTUNIT_HPP

#include "unit/unit_test.hpp"

#include "tools/varchar.hpp"

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"

class ObjectStoreTestUnit : public oos::unit_test
{
public:
  ObjectStoreTestUnit();
  virtual ~ObjectStoreTestUnit();
  
  virtual void initialize();
  virtual void finalize();
  
  void version_test();
  void optr_test();
  void expression_test();
  void access_value();
  void set_test();
  void get_test();
  void serializer();
  void ref_ptr_counter();
  void simple_object();
  void object_with_sub_object();
  void multiple_simple_objects();
  void multiple_object_with_sub_objects();
  void delete_object();
  void sub_delete();
  void hierarchy();
  void view_test();
  void clear_test();
  void generic_test();
  void test_structure();
  void test_insert();
  void test_remove();

private:
  oos::object_store ostore_;
};

#endif /* OBJECTSTORE_TESTUNIT_HPP */
