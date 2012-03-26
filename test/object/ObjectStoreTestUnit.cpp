#include "ObjectStoreTestUnit.hpp"
#include "Item.hpp"

#include "object/object_value.hpp"
#include "object/object_expression.hpp"
#include "object/object_serializer.hpp"
#include "object/object_view.hpp"

#include "tools/byte_buffer.hpp"

#include <algorithm>
#include <iostream>

using namespace oos;
using namespace std;

ObjectStoreTestUnit::ObjectStoreTestUnit()
  : unit_test("ObjectStore Test Unit")
{
  add_test("expression", std::tr1::bind(&ObjectStoreTestUnit::expression_test, this), "test object expressions");
  add_test("access", std::tr1::bind(&ObjectStoreTestUnit::access_value, this), "access object values via generic interface");
  add_test("serializer", std::tr1::bind(&ObjectStoreTestUnit::serializer, this), "serializer test");
  add_test("ref_ptr_counter", std::tr1::bind(&ObjectStoreTestUnit::ref_ptr_counter, this), "ref and ptr counter test");
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
  ostore_.insert_prototype(new object_producer<AllBase>, "ALL_BASE");
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
  typedef object_ptr<SimpleObject> simple_ptr;

  for (int i = 0; i < 10; ++i) {
    ostore_.insert(new SimpleObject("Simple", i));
  }

  variable<int, SimpleObject> x(&SimpleObject::number);
  variable<std::string, SimpleObject> y(&SimpleObject::name);

  object_view<SimpleObject> oview(ostore_);
  
  
  object_view<SimpleObject>::iterator first = oview.begin();
  object_view<SimpleObject>::iterator last = oview.end();
  while (first != last) {
    simple_ptr s(*first++);
    if ((x >= 3 && x <= 7 && x != 5)(s)) {
      std::cout << "found simple object [" << s->id() << "] with number " << s->number() << "\n";
    }
  }
  
  object_view<SimpleObject>::iterator j = std::find_if(oview.begin(), oview.end(), 6 > x);
  j = std::find_if(oview.begin(), oview.end(), x > 6);
  j = std::find_if(oview.begin(), oview.end(), x < 6);
  j = std::find_if(oview.begin(), oview.end(), x == 6);
  j = std::find_if(oview.begin(), oview.end(), 6 == x);
  j = std::find_if(oview.begin(), oview.end(), (6 == x) || (x < 4));
  j = std::find_if(oview.begin(), oview.end(), y == "Simple");
  j = std::find_if(oview.begin(), oview.end(), (x > 6) && (y == "Simple"));

  if (j != oview.end()) {
    std::cout << "found simple object [" << (*j)->id() << "] with number " << (*j)->number() << "\n";
  } else {
    std::cout << "nothing fouond\n";
  }
}

void
ObjectStoreTestUnit::serializer()
{  
  char c = 'c';
  float f = 1.55f;
  double d = 123.55789;
  int i = -98765;
  long l = -1234567890;
  unsigned u = 4567890;
  bool b = true;
  std::string title = "Hallo Welt";
  oos::varchar<64> str("The answer is 42");

  AllBase *all = new AllBase();
  
  all->set_char(c);
  all->set_float(f);
  all->set_double(d);
  all->set_int(i);
  all->set_long(l);
  all->set_unsigned(u);
  all->set_bool(b);
  all->set_string(title);
  all->set_varchar(str);
  
  object_serializer serializer;
 
  byte_buffer buffer;
  serializer.serialize(all, buffer);
  
  delete all;
  
  all = new AllBase();
  
  serializer.deserialize(all, buffer, &ostore_);

  UNIT_ASSERT_EQUAL(c, all->get_char(), "restored character is not equal to the original character");
  UNIT_ASSERT_EQUAL(f, all->get_float(), "restored float is not equal to the original float");
  UNIT_ASSERT_EQUAL(d, all->get_double(), "restored double is not equal to the original double");
  UNIT_ASSERT_EQUAL(i, all->get_int(), "restored int is not equal to the original int");
  UNIT_ASSERT_EQUAL(l, all->get_long(), "restored long is not equal to the original long");
  UNIT_ASSERT_EQUAL(u, all->get_unsigned(), "restored unsigned is not equal to the original unsigned");
  UNIT_ASSERT_EQUAL(b, all->get_bool(), "restored bool is not equal to the original bool");
  UNIT_ASSERT_EQUAL(title, all->get_string(), "restored string is not equal to the original string");
  UNIT_ASSERT_EQUAL(str, all->get_varchar(), "restored varchar is not equal to the original varchar");
  
  delete all;
}

void
ObjectStoreTestUnit::ref_ptr_counter()
{
  SimpleObject *so = new SimpleObject("Simple", 7);
  
  typedef object_ptr<SimpleObject> simple_ptr;
  typedef object_ptr<ObjectWithSubObject> withsub_ptr;
  
  simple_ptr simple = ostore_.insert(so);
  
  withsub_ptr withsub = ostore_.insert(new ObjectWithSubObject());
  withsub->simple(simple);

  
  cout << endl; 
  cout << "simple ref count: " << simple.ref_count() << "\n";
  cout << "simple ptr count: " << simple.ptr_count() << "\n";
  
  simple_ptr a1 = simple;
  simple_ptr a2 = simple;
  
  cout << "simple ref count: " << simple.ref_count() << "\n";
  cout << "simple ptr count: " << simple.ptr_count() << "\n";

  typedef object_ref<SimpleObject> simple_ref;
  
  simple_ref aref1 = a1;

  cout << "simple ref count: " << simple.ref_count() << "\n";
  cout << "simple ptr count: " << simple.ptr_count() << "\n";

  a1.reset();
  
  cout << "simple ref count: " << simple.ref_count() << "\n";
  cout << "simple ptr count: " << simple.ptr_count() << "\n";
  
  a1 = aref1;

  cout << "simple ref count: " << simple.ref_count() << "\n";
  cout << "simple ptr count: " << simple.ptr_count() << "\n";  
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
  UNIT_ASSERT_EQUAL(s, str, "retrieved value of string isn't " + str);
  UNIT_ASSERT_EQUAL(vs, vstr, "retrieved value of varchar isn't " + vstr.str());

  str = "Kambrium";
  UNIT_ASSERT_TRUE(update_value(so, "name", str), "couldn't set string value for field [name]");

  UNIT_ASSERT_TRUE(retrieve_value(so, "name", s), "couldn't get string value for field [name]");
  UNIT_ASSERT_EQUAL(s, str, "retrieved value of integer isn't " + str);

  typedef object_ptr<ObjectWithSubObject> owsub_ptr;
  typedef object_ptr<SimpleObject> osmpl_ptr;

  owsub_ptr owsub = ostore_.insert(new ObjectWithSubObject);
  osmpl_ptr osmpl = ostore_.insert(so);

//  std::cout << "inserted simple object: " << osmpl << "\n";

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

  size_t elem_size = 10000;
  // create 1000 objects
  for (size_t i = 0; i < elem_size; ++i) {
    object *o = ostore_.create("SIMPLE_OBJECT");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <SimpleObject>");
    
    SimpleObject *a = dynamic_cast<SimpleObject*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to SimpleObject");
    
    simple_ptr simple = ostore_.insert(a);
  }

  typedef object_view<SimpleObject> simple_view_t;
  simple_view_t simple_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, simple_view.size(), "expected size of view isn't 10000");
}

void
ObjectStoreTestUnit::multiple_object_with_sub_objects()
{
  typedef object_ptr<ObjectWithSubObject> ows_ptr;
    
  // create 1000 objects
  size_t elem_size = 1000;
  for (size_t i = 0; i < elem_size; ++i) {
    object *o = ostore_.create("OBJECT_WITH_SUB_OBJECT");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectWithSubObject>");
    
    ObjectWithSubObject *a = dynamic_cast<ObjectWithSubObject*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to ObjectWithSubObject");
    
    ows_ptr ows = ostore_.insert(a);
  }

  typedef object_view<ObjectWithSubObject> withsub_view_t;
  withsub_view_t withsub_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, withsub_view.size(), "expected size of view isn't 10000");
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
