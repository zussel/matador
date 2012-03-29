#include "ObjectStoreTestUnit.hpp"
#include "Item.hpp"

#include "object/object_value.hpp"
#include "object/object_expression.hpp"
#include "object/object_serializer.hpp"
#include "object/object_view.hpp"

#include "tools/byte_buffer.hpp"
#include "tools/algorithm.hpp"

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
  add_test("delete", std::tr1::bind(&ObjectStoreTestUnit::delete_object, this), "object deletion test");
  add_test("sub_delete", std::tr1::bind(&ObjectStoreTestUnit::sub_delete, this), "create and delete multiple objects with sub object");
  add_test("hierarchy", std::tr1::bind(&ObjectStoreTestUnit::hierarchy, this), "object hierarchy test");
}

ObjectStoreTestUnit::~ObjectStoreTestUnit()
{}

void
ObjectStoreTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<Item>, "ITEM");
  ostore_.insert_prototype(new object_producer<ObjectItem<Item> >, "OBJECT_ITEM");
  ostore_.insert_prototype(new object_producer<SimpleObject>, "SIMPLE_OBJECT");
  ostore_.insert_prototype(new object_producer<ObjectWithSubObject>, "OBJECT_WITH_SUB_OBJECT");
}

void
ObjectStoreTestUnit::finalize()
{
  ostore_.clear();
}

struct item_counter : public std::unary_function<const object_ptr<Item>&, void>
{
  item_counter(int &c) : count(c) {}
  
  void operator ()(const object_ptr<Item> &) { ++count; }
  int &count;
};

void
ObjectStoreTestUnit::expression_test()
{
  typedef object_ptr<Item> item_ptr;

  for (int i = 0; i < 10; ++i) {
    ostore_.insert(new Item("Simple", i));
  }

  variable<int, Item> x(&Item::get_int);
  variable<std::string, Item> y(&Item::get_string);

  object_view<Item> oview(ostore_);
  
  int count(0);
  for_each_if(oview.begin(), oview.end(), x >= 3 && x <= 7 && x != 5, item_counter(count));

  UNIT_ASSERT_EQUAL(count, 4, "invalid number of objects found");

  /*
  object_view<Item>::const_iterator first = oview.begin();
  object_view<Item>::const_iterator last = oview.end();
  while (first != last) {
    item_ptr s(*first++);
    if ((x >= 3 && x <= 7 && x != 5)(s)) {
      std::cout << "found item object [" << s->id() << "] with number " << s->get_int() << "\n";
    }
  }
  */
  
  object_view<Item>::iterator j = std::find_if(oview.begin(), oview.end(), 6 > x);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 1, "couldn't find item 1");

  j = std::find_if(oview.begin(), oview.end(), x > 6);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 7, "couldn't find item 7");

  j = std::find_if(oview.begin(), oview.end(), x < 6);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 1, "couldn't find item 1");

  j = std::find_if(oview.begin(), oview.end(), x == 6);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 6, "couldn't find item 6");

  j = std::find_if(oview.begin(), oview.end(), 6 == x);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 6, "couldn't find item 6");

  j = std::find_if(oview.begin(), oview.end(), (6 == x) || (x < 4));
  UNIT_ASSERT_EQUAL((*j)->get_int(), 1, "couldn't find item 1");

  j = std::find_if(oview.begin(), oview.end(), y == "Simple");
  UNIT_ASSERT_EQUAL((*j)->get_string(), "Simple", "couldn't find item 'Simple'");

  j = std::find_if(oview.begin(), oview.end(), (x > 6) && (y == "Simple"));
  UNIT_ASSERT_EQUAL((*j)->get_int(), 7, "couldn't find item 7");
  UNIT_ASSERT_EQUAL((*j)->get_string(), "Simple", "couldn't find item 'Simple'");
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

  Item *item = new Item();
  
  item->set_char(c);
  item->set_float(f);
  item->set_double(d);
  item->set_int(i);
  item->set_long(l);
  item->set_unsigned(u);
  item->set_bool(b);
  item->set_string(title);
  item->set_varchar(str);
  
  object_serializer serializer;
 
  byte_buffer buffer;
  serializer.serialize(item, buffer);
  
  delete item;
  
  item = new Item();
  
  serializer.deserialize(item, buffer, &ostore_);

  UNIT_ASSERT_EQUAL(c, item->get_char(), "restored character is not equal to the original character");
  UNIT_ASSERT_EQUAL(f, item->get_float(), "restored float is not equal to the original float");
  UNIT_ASSERT_EQUAL(d, item->get_double(), "restored double is not equal to the original double");
  UNIT_ASSERT_EQUAL(i, item->get_int(), "restored int is not equal to the original int");
  UNIT_ASSERT_EQUAL(l, item->get_long(), "restored long is not equal to the original long");
  UNIT_ASSERT_EQUAL(u, item->get_unsigned(), "restored unsigned is not equal to the original unsigned");
  UNIT_ASSERT_EQUAL(b, item->get_bool(), "restored bool is not equal to the original bool");
  UNIT_ASSERT_EQUAL(title, item->get_string(), "restored string is not equal to the original string");
  UNIT_ASSERT_EQUAL(str, item->get_varchar(), "restored varchar is not equal to the original varchar");
  
  delete item;
}

void
ObjectStoreTestUnit::ref_ptr_counter()
{
  Item *i = new Item("Item", 7);
  
  typedef object_ptr<Item> item_ptr;
  typedef object_ptr<ObjectItem<Item> > object_item_ptr;
  
  item_ptr item = ostore_.insert(i);
  
  object_item_ptr object_item_1 = ostore_.insert(new ObjectItem<Item>());
  object_item_ptr object_item_2 = ostore_.insert(new ObjectItem<Item>());
  object_item_1->ptr(item);

  
  unsigned long val = 0;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be null");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");

  item_ptr a1 = item;
  item_ptr a2 = item;
  
  val = 0;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be null");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");

  typedef object_ref<Item> item_ref;
  
  item_ref aref1 = a1;

  val = 0;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be null");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");

  object_item_1->ref(a1);

  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be null");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");
  
  a1 = object_item_2->ptr();

  val = 0;
  UNIT_ASSERT_EQUAL(a1.ref_count(), val, "reference count must be null");
  val = 1;
  UNIT_ASSERT_EQUAL(a1.ptr_count(), val, "pointer count must be one");

  object_item_2->ptr(item);
  val = 1;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be null");
  val = 2;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be two");
  val = 0;
  UNIT_ASSERT_EQUAL(a1.ptr_count(), val, "pointer count must be null");
  UNIT_ASSERT_EQUAL(a1.ref_count(), val, "refernce count must be null");

  object_item_2->ref(item);
  val = 2;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be two");

  object_item_2->ref(a1);
  val = 1;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be null");
  UNIT_ASSERT_EQUAL(a1.ref_count(), val, "refernce count must be null");
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
ObjectStoreTestUnit::delete_object()
{
  typedef ObjectItem<Item> TestItem;
  typedef object_ptr<TestItem> test_item_ptr;
  typedef object_ptr<Item> item_ptr;

  
  item_ptr item = ostore_.insert(new Item("item 1"));

  TestItem *ti = new TestItem;
  ti->ref(item);
  
  test_item_ptr testitem = ostore_.insert(ti);

  UNIT_ASSERT_EQUAL(item.ref_count(), (unsigned long)1, "reference count for item should be 1 (one)");
  UNIT_ASSERT_EQUAL(item.ptr_count(), (unsigned long)0, "reference count for item should be 0 (zero)");
  UNIT_ASSERT_EQUAL(testitem.ref_count(), (unsigned long)0, "reference count for test item should be 0 (zero)");
  UNIT_ASSERT_EQUAL(testitem.ptr_count(), (unsigned long)0, "reference count for test item should be 0 (zero)");
  
  typedef object_view<Item> item_view_t;
  item_view_t item_view(ostore_);

  UNIT_ASSERT_FALSE(ostore_.remove(item), "item shouldn't be removable because ref count is one");

  UNIT_ASSERT_TRUE(ostore_.remove(testitem), "test item must be removable");
  
  UNIT_ASSERT_TRUE(ostore_.remove(item), "item must be removable");
}

void
ObjectStoreTestUnit::sub_delete()
{
}

void
ObjectStoreTestUnit::hierarchy()
{
  ostore_.insert_prototype<ItemA>("ITEM_A", "false", "ITEM");
  ostore_.insert_prototype<ItemB>("ITEM_B", "false", "ITEM");
  ostore_.insert_prototype<ItemC>("ITEM_C", "false", "ITEM");
  
  /* Insert 5 object of each item
   * object type
   */

  Item *itm;
  for (int i = 0; i < 5; ++i) {
    std::stringstream str;
    str << "item " << i;
    ostore_.insert(new Item(str.str()));
    str.str("");
    str << "item a " << i;
    itm = new ItemA;
    itm->set_string(str.str());
    ostore_.insert(itm);
    str.str("");
    str << "item b " << i;
    itm = new ItemB;
    itm->set_string(str.str());
    ostore_.insert(itm);
    str.str("");
    str << "item c " << i;
    itm = new ItemC;
    itm->set_string(str.str());
    ostore_.insert(itm);
  }
  
  typedef object_view<Item> item_view_t;
  typedef object_ptr<Item> item_ptr;

  item_view_t item_view(ostore_);

  /************************************
   * 
   * list all items with sub items
   *
   ************************************/

  size_t max = 20;
  UNIT_ASSERT_EQUAL(item_view.size(), max, "expected item view size isn't 15");
  
  item_view_t::const_iterator first = item_view.begin();
  item_view_t::const_iterator last = item_view.end();

  int count = 0;
  while (first != last) {
    UNIT_ASSERT_LESS(count, 20, "item view count isn't valid");
    ++first;
    ++count;
  }

  item_view.skip_siblings(true);

  /************************************
   * 
   * list all items without sub items
   *
   ************************************/

  max = 5;
  UNIT_ASSERT_EQUAL(item_view.size(), max, "expected item view size isn't 15");
  
  first = item_view.begin();
  last = item_view.end();
  
  count = 0;
  while (first != last) {
    UNIT_ASSERT_LESS(count, 5, "item view count isn't valid");
    ++first;
    ++count;
  }
  
  // Todo: check deletion
  
  // Todo: check sub item view (a,b,c)
  typedef object_view<ItemA> item_a_view_t;
  typedef object_ptr<ItemA> item_a_ptr;

  item_a_view_t item_a_view(ostore_);

  UNIT_ASSERT_EQUAL(item_a_view.size(), max, "expected item_a view size isn't 15");
  
  item_a_view_t::const_iterator afirst = item_a_view.begin();
  item_a_view_t::const_iterator alast = item_a_view.end();

  count = 0;
  while (afirst != alast) {
    UNIT_ASSERT_LESS(count, 5, "item_a view count isn't valid");
    ++afirst;
    ++count;
  }
}
