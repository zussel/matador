#include "ObjectStoreTestUnit.hpp"

using namespace oos;

ObjectStoreTestUnit::ObjectStoreTestUnit()
  : unit_test("ObjectStore Test Unit")
{
  add_test("simple", std::tr1::bind(&ObjectStoreTestUnit::simple_object, this), "create and delete one object");
  add_test("with_sub", std::tr1::bind(&ObjectStoreTestUnit::object_with_sub_object, this), "create and delete object with sub object");
  add_test("multiple_simple", std::tr1::bind(&ObjectStoreTestUnit::multiple_simple_objects, this), "create and delete multiple objects");
  add_test("multiple_object_with_sub", std::tr1::bind(&ObjectStoreTestUnit::multiple_object_with_sub_objects, this), "create and delete multiple objects with sub object");
}

ObjectStoreTestUnit::~ObjectStoreTestUnit()
{}

void
ObjectStoreTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<SimpleObject>, "SIMPLE_OBJECT");
  ostore_.insert_prototype(new object_producer<ObjectWithSubObject>, "OBJECT_WITH_SUB_OBJECT");
}

void
ObjectStoreTestUnit::finalize()
{
  ostore_.clear();
}

void
ObjectStoreTestUnit::simple_object()
{
  object *o = ostore_.create("SIMPLE_OBJECT");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
  
  SimpleObject *a = dynamic_cast<SimpleObject*>(o);
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
  
  typedef object_ptr<SimpleObject> simple_ptr;
  
  simple_ptr simple = ostore_.insert(a);
  
  UNIT_ASSERT_NOT_NULL(simple.get(), "simple object insertion failed");
  
  UNIT_ASSERT_TRUE(ostore_.remove(simple), "deletion of simple failed");
}

void
ObjectStoreTestUnit::object_with_sub_object()
{
  object *o = ostore_.create("OBJECT_WITH_SUB_OBJECT");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectWithSubObject>");
  
  ObjectWithSubObject *s = dynamic_cast<ObjectWithSubObject*>(o);
  
  UNIT_ASSERT_NOT_NULL(s, "couldn't cast object to ObjectWithSubObject");
  
  typedef object_ptr<ObjectWithSubObject> obj_with_sub_ptr;
  
  obj_with_sub_ptr ows = ostore_.insert(s);
  
  UNIT_ASSERT_NOT_NULL(ows.get(), "object with sub object insertion failed");
  
  // check if sub object exists
  object_ptr<SimpleObject> simple = ows->simple();
  
  UNIT_ASSERT_NOT_NULL(simple.get(), "simple sub object creation failed");
  
  UNIT_ASSERT_TRUE(ostore_.remove(ows), "deletion of object with sub object failed");
}

void
ObjectStoreTestUnit::multiple_simple_objects()
{
  typedef object_ptr<SimpleObject> simple_ptr;
    
  // create 1000 objects
  UNIT_INFO("\ncreating objects :");
  for (int i = 0; i < 100000; ++i) {
    if (i%1000 == 0) {
      std::cout << " " << i;
      std::cout.flush();
    }
    object *o = ostore_.create("SIMPLE_OBJECT");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
    
    SimpleObject *a = dynamic_cast<SimpleObject*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
    
    simple_ptr simple = ostore_.insert(a);
    
    //ostore_.remove(simple);
  }
  UNIT_INFO(" done\n");
  
  /*
  std::cout << std::endl;
  SimpleObject *pso;
  std::cout << "typeid (pso*)   : " << typeid(pso).name() << std::endl;
  std::cout << "typeid (SimpleObject*)   : " << typeid(SimpleObject*).name() << std::endl;
  std::cout << "typeid (SimpleObject)    : " << typeid(SimpleObject).name() << std::endl;
  SimpleObject so;
  std::cout << "typeid (SimpleObject  so): " << typeid(so).name() << std::endl;
  std::cout << "typeid (SimpleObject *so): " << typeid(&so).name() << std::endl;
  */
}

void
ObjectStoreTestUnit::multiple_object_with_sub_objects()
{
  typedef object_ptr<ObjectWithSubObject> ows_ptr;
    
  // create 1000 objects
  UNIT_INFO("\ncreating objects :");
  for (int i = 0; i < 100000; ++i) {
    if (i%1000 == 0) {
      std::cout << " " << i;
      std::cout.flush();
    }

    object *o = ostore_.create("OBJECT_WITH_SUB_OBJECT");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectWithSubObject>");
    
    ObjectWithSubObject *a = dynamic_cast<ObjectWithSubObject*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to ObjectWithSubObject");
    
    ows_ptr ows = ostore_.insert(a);
    
    //ostore_.remove(simple);
  }
  UNIT_INFO(" done\n");
}
