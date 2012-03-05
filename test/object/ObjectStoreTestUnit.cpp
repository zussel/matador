#include "ObjectStoreTestUnit.hpp"

#include "object/object_value.hpp"
#include "object/object_expression.hpp"
#include "object/object_view.hpp"

#include <algorithm>

using namespace oos;

ObjectStoreTestUnit::ObjectStoreTestUnit()
  : unit_test("ObjectStore Test Unit")
{
  add_test("expression", std::tr1::bind(&ObjectStoreTestUnit::expression_test, this), "test object expressions");
  add_test("access", std::tr1::bind(&ObjectStoreTestUnit::access_value, this), "access object values via generic interface");
  add_test("simple", std::tr1::bind(&ObjectStoreTestUnit::simple_object, this), "create and delete one object");
  add_test("with_sub", std::tr1::bind(&ObjectStoreTestUnit::object_with_sub_object, this), "create and delete object with sub object");
  add_test("multiple_simple", std::tr1::bind(&ObjectStoreTestUnit::multiple_simple_objects, this), "create and delete multiple objects");
  add_test("multiple_object_with_sub", std::tr1::bind(&ObjectStoreTestUnit::multiple_object_with_sub_objects, this), "create and delete multiple objects with sub object");
  add_test("sub_delete", std::tr1::bind(&ObjectStoreTestUnit::sub_delete, this), "create and delete multiple objects with sub object");
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
ObjectStoreTestUnit::expression_test()
{
  for (int i = 0; i < 10; ++i) {
    ostore_.insert(new SimpleObject("Simple", i));
  }

  variable<int, SimpleObject> x(&SimpleObject::number);
  variable<std::string, SimpleObject> y(&SimpleObject::name);

  object_view<SimpleObject> oview(ostore_);
  
  object_view<SimpleObject>::iterator j = std::find_if(oview.begin(), oview.end(), 6 > x);
  j = std::find_if(oview.begin(), oview.end(), x > 6);
  j = std::find_if(oview.begin(), oview.end(), x < 6);
  j = std::find_if(oview.begin(), oview.end(), x == 6);
  j = std::find_if(oview.begin(), oview.end(), 6 == x);
  j = std::find_if(oview.begin(), oview.end(), (6 == x) || (x < 4));
  j = std::find_if(oview.begin(), oview.end(), y == "Simple");
  j = std::find_if(oview.begin(), oview.end(), (x > 6) && (y != "Simple"));

  if (j != oview.end()) {
    std::cout << "found simple object [" << (*j)->id() << "] with number " << (*j)->number() << "\n";
  } else {
    std::cout << "nothing fouond\n";
  }
}

void
ObjectStoreTestUnit::access_value()
{
  std::string str("Godwanda");
  varchar<32> vstr("Godwanda");

  int numb = 42;

  SimpleObject *so = new SimpleObject(str, numb);

  int n;

  UNIT_ASSERT_TRUE(retrieve_value(so, "number", n), "couldn't get integer value for field [number]");
  UNIT_ASSERT_EQUAL(n, numb, "retrieved value of integer isn't " + numb);

  numb = 13;
  UNIT_ASSERT_TRUE(update_value(so, "number", numb), "couldn't set integer value for field [number]");

  UNIT_ASSERT_TRUE(retrieve_value(so, "number", n), "couldn't get integer value for field [number]");
  UNIT_ASSERT_EQUAL(n, numb, "retrieved value of integer isn't " + numb);

  std::string s;

  oos::varchar<32> vs;

  UNIT_ASSERT_TRUE(retrieve_value(so, "name", vs), "couldn't get varchar value for field [name]");
  UNIT_ASSERT_TRUE(retrieve_value(so, "name", s), "couldn't get string value for field [name]");
  UNIT_ASSERT_EQUAL(s, str, "retrieved value of integer isn't " + str);
  UNIT_ASSERT_EQUAL(vs, vstr, "retrieved value of integer isn't " + vstr.str());

  str = "Kambrium";
  UNIT_ASSERT_TRUE(update_value(so, "name", str), "couldn't set string value for field [name]");

  UNIT_ASSERT_TRUE(retrieve_value(so, "name", s), "couldn't get string value for field [name]");
  UNIT_ASSERT_EQUAL(s, str, "retrieved value of integer isn't " + str);

  typedef object_ptr<ObjectWithSubObject> owsub_ptr;
  typedef object_ptr<SimpleObject> osmpl_ptr;

  owsub_ptr owsub = ostore_.insert(new ObjectWithSubObject);
  osmpl_ptr osmpl = ostore_.insert(so);

  std::cout << "inserted simple object: " << osmpl << "\n";

  UNIT_ASSERT_TRUE(update_value(owsub, "simple", osmpl), "couldn't set object field [simple]");

  osmpl.reset();

  UNIT_ASSERT_TRUE(retrieve_value(owsub, "simple", osmpl), "couldn't get object field [simple]");
  UNIT_ASSERT_EQUAL(osmpl.get(), so, "retrieved object isn't the expected");
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
  
//  UNIT_ASSERT_FALSE(ostore_.remove(ows), "deletion of object with sub object unexpectedly succeeded");
  // clear simple optr
//  simple.reset();
  // try remove again
  UNIT_ASSERT_TRUE(ostore_.remove(ows), "deletion of object with sub object failed");
}

void
ObjectStoreTestUnit::multiple_simple_objects()
{
  typedef object_ptr<SimpleObject> simple_ptr;
    
  // create 1000 objects
  UNIT_INFO("\ncreating objects :");
  for (int i = 0; i < 10000; ++i) {
    if (i%100 == 0) {
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
  for (int i = 0; i < 1000; ++i) {
    if (i%10 == 0) {
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

void
ObjectStoreTestUnit::sub_delete()
{
  typedef object_ptr<ObjectWithSubObject> with_sub_ptr;
  typedef object_ptr<SimpleObject> simple_ptr;
  
  with_sub_ptr ws1 = ostore_.insert(new ObjectWithSubObject);
  with_sub_ptr ws2 = ostore_.insert(new ObjectWithSubObject);
  
  simple_ptr s1 = ws1->simple();
  simple_ptr s2 = ws2->simple();
  
  std::cout << std::endl;
  std::cout << "s1 ref count: " << s1.ref_count() << "\n";
  std::cout << "s1 ptr count: " << s1.ptr_count() << "\n";
  std::cout << "s2 ref count: " << s2.ref_count() << "\n";
  std::cout << "s2 ptr count: " << s2.ptr_count() << "\n";
  
  ws2->simple(s1);

  std::cout << "s1 ref count: " << s1.ref_count() << "\n";
  std::cout << "s1 ptr count: " << s1.ptr_count() << "\n";
  std::cout << "s2 ref count: " << s2.ref_count() << "\n";
  std::cout << "s2 ptr count: " << s2.ptr_count() << "\n";
}
