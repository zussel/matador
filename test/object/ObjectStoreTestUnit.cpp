#include "ObjectStoreTestUnit.hpp"
#include "../Item.hpp"

#include "object/object_expression.hpp"
#include "object/object_serializer.hpp"
#include "object/object_view.hpp"
#include "object/generic_access.hpp"

#include "tools/algorithm.hpp"
#include "tools/date.hpp"
#include "tools/time.hpp"

#include "version.hpp"

#include <iostream>

using namespace oos;
using namespace std;

ObjectStoreTestUnit::ObjectStoreTestUnit()
  : unit_test("store", "ObjectStore Test Unit")
{
  add_test("version", std::bind(&ObjectStoreTestUnit::version_test, this), "test oos version");
  add_test("optr", std::bind(&ObjectStoreTestUnit::optr_test, this), "test optr behaviour");
  add_test("expression", std::bind(&ObjectStoreTestUnit::expression_test, this), "test serializable expressions");
  add_test("set", std::bind(&ObjectStoreTestUnit::set_test, this), "access serializable values via set interface");
  add_test("get", std::bind(&ObjectStoreTestUnit::get_test, this), "access serializable values via get interface");
  add_test("serializer", std::bind(&ObjectStoreTestUnit::serializer, this), "serializer test");
  add_test("ref_ptr_counter", std::bind(&ObjectStoreTestUnit::ref_ptr_counter, this), "ref and ptr counter test");
  add_test("simple", std::bind(&ObjectStoreTestUnit::simple_object, this), "create and delete one object");
  add_test("with_sub", std::bind(&ObjectStoreTestUnit::object_with_sub_object, this), "create and delete serializable with sub object");
  add_test("multiple_simple", std::bind(&ObjectStoreTestUnit::multiple_simple_objects, this), "create and delete multiple objects");
  add_test("multiple_object_with_sub", std::bind(&ObjectStoreTestUnit::multiple_object_with_sub_objects, this), "create and delete multiple objects with sub object");
  add_test("delete", std::bind(&ObjectStoreTestUnit::delete_object, this), "serializable deletion test");
  add_test("hierarchy", std::bind(&ObjectStoreTestUnit::hierarchy, this), "serializable hierarchy test");
  add_test("view", std::bind(&ObjectStoreTestUnit::view_test, this), "serializable view test");
  add_test("clear", std::bind(&ObjectStoreTestUnit::clear_test, this), "serializable store clear test");
  add_test("generic", std::bind(&ObjectStoreTestUnit::generic_test, this), "generic serializable access test");
  add_test("structure", std::bind(&ObjectStoreTestUnit::test_structure, this), "serializable transient structure test");
  add_test("structure_cyclic", std::bind(&ObjectStoreTestUnit::test_structure_cyclic, this), "serializable transient cyclic structure test");
  add_test("structure_container", std::bind(&ObjectStoreTestUnit::test_structure_container, this), "serializable transient container structure test");
  add_test("transient_optr", std::bind(&ObjectStoreTestUnit::test_transient_optr, this), "test transient object pointer");
  add_test("insert", std::bind(&ObjectStoreTestUnit::test_insert, this), "serializable insert test");
  add_test("remove", std::bind(&ObjectStoreTestUnit::test_remove, this), "serializable remove test");
  add_test("pk", std::bind(&ObjectStoreTestUnit::test_primary_key, this), "serializable proxy primary key test");
//  add_test("to_many", std::bind(&ObjectStoreTestUnit::test_to_many, this), "to many test");
}

ObjectStoreTestUnit::~ObjectStoreTestUnit()
{}

typedef List<oos::object_ptr<ObjectItem<Item> > > ObjectItemPtrList;

void
ObjectStoreTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("item");
  ostore_.insert_prototype<ObjectItem<Item> >("object_item");
  ostore_.insert_prototype(new list_object_producer<ItemPtrList>("ptr_list"), "item_ptr_list");
  ostore_.insert_prototype(new list_object_producer<ObjectItemPtrList>("object_ptr_list"), "object_item_ptr_list");

  ostore_.insert_prototype<person>("person");
  ostore_.insert_prototype<employee, person>("employee");
  ostore_.insert_prototype<department>("department");
}

void
ObjectStoreTestUnit::finalize()
{
  ostore_.clear(true);
}

struct item_counter : public std::unary_function<const object_ptr<ObjectItem<Item> >&, void>
{
  item_counter(int &c) : count(c) {}
  
  void operator ()(const object_ptr<ObjectItem<Item> > &) { ++count; }
  int &count;
};

void
ObjectStoreTestUnit::version_test()
{
  std::string version("0.2.1");
  
  UNIT_ASSERT_EQUAL(oos::version::str, version, "invalid oos version");

  UNIT_ASSERT_EQUAL(oos::version::major, 0, "invalid major version");
  UNIT_ASSERT_EQUAL(oos::version::minor, 2, "invalid minor version");
  UNIT_ASSERT_EQUAL(oos::version::patch_level, 1, "invalid patch level");
}


void ObjectStoreTestUnit::optr_test()
{
  typedef object_ptr<Item> item_ptr;

  item_ptr item_null;

  UNIT_ASSERT_EXCEPTION(ostore_.insert(item_null), object_exception, "serializable pointer is null", "shouldn't insert null serializable pointer");

  item_ptr item(new Item("Test"));

  UNIT_ASSERT_NULL(item.store(), "item must not be internal");

  item = ostore_.insert(item);

  UNIT_ASSERT_NOT_NULL(item.store(), "item must be internal");
}

void
ObjectStoreTestUnit::expression_test()
{
  typedef object_ptr<ObjectItem<Item> > object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  typedef object_ptr<ObjectItemPtrList> itemlist_ptr;

  itemlist_ptr itemlist = ostore_.insert(new ObjectItemPtrList("object_ptr_list"));

  item_ptr ii;
  for (int i = 0; i < 10; ++i) {
    object_item_ptr oi = ostore_.insert(new ObjectItem<Item>("ObjectItem", i));
    ii = ostore_.insert(new Item("Item", i));
    oi->ptr(ii);
    itemlist->push_back(oi);
  }

  variable<int> x(make_var(&ObjectItem<Item>::get_int));
  variable<std::string> y(make_var(&ObjectItem<Item>::get_string));
  variable<object_ptr<Item> > u(make_var(&ObjectItem<Item>::ptr));

  object_view<ObjectItem<Item> > oview(ostore_);
  
  int count(0);
  for_each_if(oview.begin(), oview.end(), x >= 3 && x <= 7 && x != 5, item_counter(count));

  UNIT_ASSERT_EQUAL(count, 4, "invalid number of objects found");

  typedef ObjectItemPtrList::item_type ObjectItemType;

  //Clang 3.2 needs the explicit template parameters on make_var
  variable<int> z(make_var<int, ObjectItemType, object_item_ptr>(&ObjectItemType::value, &ObjectItem<Item>::get_int));

  ObjectItemPtrList::const_iterator it = std::find_if(itemlist->begin(), itemlist->end(), z == 4);
  UNIT_ASSERT_FALSE(it == itemlist->end(), "couldn't find item");

  object_view<ObjectItem<Item> >::iterator j = std::find_if(oview.begin(), oview.end(), 6 > x);
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

  j = std::find_if(oview.begin(), oview.end(), u == ii);
  UNIT_ASSERT_EQUAL((*j)->ptr(), ii, "couldn't find item 10");

  j = std::find_if(oview.begin(), oview.end(), y == std::string("ObjectItem"));
  UNIT_ASSERT_EQUAL((*j)->get_string(), "ObjectItem", "couldn't find item 'ObjectItem'");

  // try to find unknown object
  j = std::find_if(oview.begin(), oview.end(), y == std::string("Simple"));
  UNIT_ASSERT_TRUE(j == oview.end(), "iterator must be end");

  j = std::find_if(oview.begin(), oview.end(), (x > 6) && (y == std::string("ObjectItem")));
  UNIT_ASSERT_EQUAL((*j)->get_int(), 7, "couldn't find item 7");
  UNIT_ASSERT_EQUAL((*j)->get_string(), "ObjectItem", "couldn't find item 'ObjectItem'");
}

void
ObjectStoreTestUnit::serializer()
{  
  char c = 'c';
  float f = 1.55f;
  double d = 123.55789;
  short s = -42;
  int i = -98765;
  long l = -1234567890;
  unsigned short us = 45;
  unsigned int ui = 4567890;
  unsigned long ul = 987654321;
  bool b = true;
  std::string title = "Hallo Welt";
  oos::varchar<64> str("The answer is 42");
  oos::date dt(15, 9, 1972);
  oos::time t(2008, 12, 27, 13, 6, 57, 4711);

  Item *item = new Item();
  
  item->set_char(c);
  item->set_float(f);
  item->set_double(d);
  item->set_short(s);
  item->set_int(i);
  item->set_long(l);
  item->set_unsigned_short(us);
  item->set_unsigned_int(ui);
  item->set_unsigned_long(ul);
  item->set_bool(b);
  item->set_string(title);
  item->set_varchar(str);
  item->set_date(dt);
  item->set_time(t);
  
  object_serializer serializer;
 
  byte_buffer buffer;
  serializer.serialize(item, &buffer);
  
  delete item;
  
  item = new Item();
  
  serializer.deserialize(item, &buffer, &ostore_);

  UNIT_ASSERT_EQUAL(c, item->get_char(), "restored character is not equal to the original character");
  UNIT_ASSERT_EQUAL(f, item->get_float(), "restored float is not equal to the original float");
  UNIT_ASSERT_EQUAL(d, item->get_double(), "restored double is not equal to the original double");
  UNIT_ASSERT_EQUAL(s, item->get_short(), "restored short is not equal to the original short");
  UNIT_ASSERT_EQUAL(i, item->get_int(), "restored int is not equal to the original int");
  UNIT_ASSERT_EQUAL(l, item->get_long(), "restored long is not equal to the original long");
  UNIT_ASSERT_EQUAL(us, item->get_unsigned_short(), "restored unsigned short is not equal to the original unsigned");
  UNIT_ASSERT_EQUAL(ui, item->get_unsigned_int(), "restored unsigned int is not equal to the original unsigned");
  UNIT_ASSERT_EQUAL(ul, item->get_unsigned_long(), "restored unsigned long is not equal to the original unsigned");
  UNIT_ASSERT_EQUAL(b, item->get_bool(), "restored bool is not equal to the original bool");
  UNIT_ASSERT_EQUAL(title, item->get_string(), "restored string is not equal to the original string");
  UNIT_ASSERT_EQUAL(str, item->get_varchar(), "restored varchar is not equal to the original varchar");
  UNIT_ASSERT_EQUAL(dt, item->get_date(), "restored date is not equal to the original date");
  UNIT_ASSERT_EQUAL(t, item->get_time(), "restored time is not equal to the original time");

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
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be zero");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");

  item_ptr a1 = item;
  item_ptr a2 = item;
  
  val = 0;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be zero");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");

  typedef object_ref<Item> item_ref;
  
  item_ref aref1 = a1;

  val = 0;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be zero");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");

  object_item_1->ref(a1);

  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be one");
  val = 1;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be one");
  
  a1 = object_item_2->ptr();

  val = 0;
  UNIT_ASSERT_EQUAL(a1.ref_count(), val, "reference count must be zero");
  UNIT_ASSERT_EQUAL(a1.ptr_count(), val, "pointer count must be zero");

  object_item_2->ptr(item);
  val = 1;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be one");
  val = 2;
  UNIT_ASSERT_EQUAL(item.ptr_count(), val, "pointer count must be two");
  val = 0;
  UNIT_ASSERT_EQUAL(a1.ptr_count(), val, "pointer count must be zero");
  UNIT_ASSERT_EQUAL(a1.ref_count(), val, "refernce count must be zero");

  object_item_2->ref(item);
  val = 2;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be two");

  object_item_2->ref(a1);
  val = 1;
  UNIT_ASSERT_EQUAL(item.ref_count(), val, "reference count must be one");
  val = 0;
  UNIT_ASSERT_EQUAL(a1.ref_count(), val, "refernce count must be zero");
}


void
ObjectStoreTestUnit::set_test()
{
  Item *i = new Item("item", 4711);
  
  oos::set(i, "val_int", 1);

  UNIT_ASSERT_EQUAL(1, i->get_int(), "value must be one (1)");

  delete i;
}

void
ObjectStoreTestUnit::get_test()
{
}

void
ObjectStoreTestUnit::simple_object()
{
  serializable *o = ostore_.create("item");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create serializable of type <Item>");
  
  Item *a = dynamic_cast<Item*>(o);
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't cast serializable to Item");
  
  typedef object_ptr<Item> item_ptr;
  
  item_ptr simple = ostore_.insert(a);
  
  UNIT_ASSERT_NOT_NULL(simple.get(), "item serializable insertion failed");
  
  UNIT_ASSERT_TRUE(ostore_.is_removable(simple), "deletion of item serializable failed");
  
  ostore_.remove(simple);
}

void
ObjectStoreTestUnit::object_with_sub_object()
{
  serializable *o = ostore_.create("object_item");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create serializable of type <ObjectItem>");
  
  ObjectItem<Item> *s = dynamic_cast<ObjectItem<Item>*>(o);
  
  UNIT_ASSERT_NOT_NULL(s, "couldn't cast serializable to ObjectItem<Item>");
  
  typedef object_ptr<ObjectItem<Item> > obj_item_ptr;
  
  obj_item_ptr ows = ostore_.insert(s);
  
  UNIT_ASSERT_NOT_NULL(ows.get(), "serializable item serializable insertion failed");
  
  // check if sub serializable exists
  object_ptr<Item> simple = ows->ptr();
  
  UNIT_ASSERT_NULL(simple.get(), "item must be nullptr");
  
  UNIT_ASSERT_TRUE(ostore_.is_removable(ows), "deletion of serializable item failed");
  
  ostore_.remove(ows);
}

void
ObjectStoreTestUnit::multiple_simple_objects()
{
  typedef object_ptr<Item> item_ptr;

  size_t elem_size = 10;
  // create 10 objects
  for (size_t i = 0; i < elem_size; ++i) {
    serializable *o = ostore_.create("item");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create serializable of type <Item>");
    
    Item *a = dynamic_cast<Item*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast serializable to Item");
    
    item_ptr simple = ostore_.insert(a);
  }

  typedef object_view<Item> simple_view_t;
  simple_view_t simple_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, simple_view.size(), "expected size of view isn't 10");
}

void
ObjectStoreTestUnit::multiple_object_with_sub_objects()
{
  typedef object_ptr<ObjectItem<Item> > ows_ptr;
    
  // create 10 objects
  size_t elem_size = 10;
  for (size_t i = 0; i < elem_size; ++i) {
    serializable *o = ostore_.create("object_item");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create serializable of type <ObjectItem<Item> >");
    
    ObjectItem<Item>  *a = dynamic_cast<ObjectItem<Item> *>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast serializable to ObjectItem<Item> ");
    
    ows_ptr ows = ostore_.insert(a);
  }

  typedef object_view<ObjectItem<Item> > withsub_view_t;
  withsub_view_t withsub_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, withsub_view.size(), "expected size of view isn't 10");
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

  UNIT_ASSERT_FALSE(ostore_.is_removable(item), "item shouldn't be removable because ref count is one");

  try {
    ostore_.remove(item);
  } catch(object_exception &) {
  }

  UNIT_ASSERT_TRUE(ostore_.is_removable(testitem), "test item must be removable");
  
  ostore_.remove(testitem);

  UNIT_ASSERT_TRUE(ostore_.is_removable(item), "item must be removable");
  
  ostore_.remove(item);
}

void
ObjectStoreTestUnit::hierarchy()
{
  ostore_.insert_prototype<ItemA, Item>("ITEM_A");
  ostore_.insert_prototype<ItemB, Item>("ITEM_B");
  ostore_.insert_prototype<ItemC, Item>("ITEM_C");
  
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

void
ObjectStoreTestUnit::view_test()
{
  for (int i = 0; i < 10; ++i) {
    std::stringstream str;
    str << "Item " << i+1;
    ostore_.insert(new Item(str.str(), i+1));
  }

  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> item_view_t;

  item_view_t iview(ostore_);

  UNIT_ASSERT_EQUAL((int)iview.size(), 10, "invalid item view size");
  UNIT_ASSERT_FALSE(iview.empty(), "item view is not empty");

  item_ptr item = iview.front();

  UNIT_ASSERT_GREATER(item->id(), 0UL, "invalid item");

  item = iview.back();
  
  UNIT_ASSERT_GREATER(item->id(), 0UL, "invalid item");
}

void
ObjectStoreTestUnit::clear_test()
{
  for (int i = 0; i < 10; ++i) {
    std::stringstream str;
    str << "Item " << i+1;
    ostore_.insert(new Item(str.str(), i+1));
  }

  typedef object_view<Item> item_view_t;

  item_view_t iview(ostore_);

  UNIT_ASSERT_EQUAL((int)iview.size(), 10, "invalid item view size");
  UNIT_ASSERT_FALSE((int)iview.empty(), "item view shouldn't be empty");
  UNIT_ASSERT_FALSE((int)ostore_.empty(), "serializable store shouldn't be empty");

  ostore_.clear();

  UNIT_ASSERT_TRUE((int)ostore_.empty(), "serializable store must be empty");
  UNIT_ASSERT_EQUAL((int)iview.size(), 0, "invalid item view size");
  UNIT_ASSERT_TRUE((int)iview.empty(), "item view must be empty");

  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();

  UNIT_ASSERT_FALSE(first == last, "prototype iterator shouldn't be the same");

  ostore_.clear(true);

  first = ostore_.begin();
  last = ostore_.end();

  UNIT_ASSERT_TRUE(first == last, "prototype iterator must be the same");
}

struct basic_test_pair
{
  std::string str_result;
};

template < class T, class Enabled = void >
struct test_pair;

template < class T >
struct test_pair<T, typename std::enable_if< !std::is_same<T, char*>::value >::type > : public basic_test_pair
{
  explicit test_pair(const T &exp) : expected(exp) {}
  T expected;
  T result;
};

template < class T >
struct test_pair<T, typename std::enable_if< std::is_same<T, char*>::value >::type > : public basic_test_pair
{
  explicit test_pair(const char exp[],int s)
    : expected(exp)
    , result(new char[s])
    , expected_size(strlen(exp))
    , size(s)
  {}
  ~test_pair() { delete [] result; }
  const char* expected;
  T result;
  int expected_size;
  int size;
};

void
ObjectStoreTestUnit::generic_test()
{
  test_pair<char> c('c');
  test_pair<bool> b(true);
  test_pair<float> f(1.55f);
  test_pair<double> d(123.55789);
  test_pair<short> s(-42);
  test_pair<int> i(-98765);
  test_pair<long> l(1234567890);
  test_pair<unsigned short> us(45);
  test_pair<unsigned int> ui(4567890);
  test_pair<unsigned long> ul(987654321);
  test_pair<char*> cstr("baba", 256);
  test_pair<std::string> str("Hallo Welt");
  test_pair<oos::varchar<64> > varstr("The answer is 42");
  test_pair<oos::date> dateval(oos::date("29.4.1972"));
  test_pair<oos::time > timeval(oos::time(2015, 10, 16, 8, 54, 32, 123));

//  char c = 'c';
//  float f = 1.55f;
//  double d = 123.55789;
//  short s = -42;
//  int i = -98765;
//  long l = -1234567890;
//  unsigned short us = 45;
//  unsigned int ui = 4567890;
//  unsigned long ul = 987654321;
//  bool b = true;
//  const char *cstr("baba");
//  std::string title = "Hallo Welt";
//  oos::varchar<64> str("The answer is 42");

  std::unique_ptr<Item> item(new Item());

  oos::set(item, "val_char", c.expected);
  oos::get(item, "val_char", c.result);
  UNIT_ASSERT_EQUAL(c.result, c.expected, "not expected result value");

  oos::set(item, "val_short", s.expected);
  oos::get(item, "val_short", s.result);
  UNIT_ASSERT_EQUAL(s.result, s.expected, "not expected result value");
  oos::get(item, "val_short", s.str_result);
  UNIT_ASSERT_EQUAL(s.str_result, "-42", "float string is invalid");

  oos::set(item, "val_int", i.expected);
  oos::get(item, "val_int", i.result);
  UNIT_ASSERT_EQUAL(i.result, i.expected, "not expected result value");

  oos::set(item, "val_long", l.expected);
  oos::get(item, "val_long", l.result);
  UNIT_ASSERT_EQUAL(l.result, l.expected, "not expected result value");

  oos::set(item, "val_unsigned_short", us.expected);
  oos::get(item, "val_unsigned_short", us.result);
  UNIT_ASSERT_EQUAL(us.result, us.expected, "not expected result value");

  oos::set(item, "val_unsigned_int", ui.expected);
  oos::get(item, "val_unsigned_int", ui.result);
  UNIT_ASSERT_EQUAL(ui.result, ui.expected, "not expected result value");

  oos::set(item, "val_unsigned_long", ul.expected);
  oos::get(item, "val_unsigned_long", ul.result);
  UNIT_ASSERT_EQUAL(ul.result, ul.expected, "not expected result value");

  oos::set(item, "val_bool", b.expected);
  oos::get(item, "val_bool", b.result);
  UNIT_ASSERT_EQUAL(b.result, b.expected, "not expected result value");

  oos::set(item, "val_cstr", cstr.expected, cstr.expected_size);
  oos::get(item, "val_cstr", cstr.result, cstr.size);
  UNIT_ASSERT_EQUAL(cstr.result, cstr.expected, "not expected result value");

  oos::set(item, "val_string", str.expected);
  oos::get(item, "val_string", str.result);
  UNIT_ASSERT_EQUAL(str.result, str.expected, "not expected result value");

  oos::set(item, "val_varchar", varstr.expected);
  oos::get(item, "val_varchar", varstr.result);
  UNIT_ASSERT_EQUAL(varstr.result, varstr.expected, "not expected result value");

  oos::set(item, "val_float", f.expected);
  oos::get(item, "val_float", f.result);
  UNIT_ASSERT_EQUAL(f.result, f.expected, "not expected result value");
  /* get float value into string
   * with precision 2
   */
  oos::get(item, "val_float", f.str_result, 2);
  UNIT_ASSERT_EQUAL(f.str_result, "1.55", "float string is invalid");

  oos::set(item, "val_double", d.expected);
  oos::get(item, "val_double", d.result);
  UNIT_ASSERT_EQUAL(d.result, d.expected, "not expected result value");
  /* get double value into string
   * with precision 3
   */
  oos::get(item, "val_double", d.str_result, 3);
  UNIT_ASSERT_EQUAL(d.str_result, "123.558", "double string is invalid");

  oos::get(item, "val_int", str.result);
  UNIT_ASSERT_EQUAL(str.result, "-98765", "float string is invalid");

  oos::set(item, "val_date", dateval.expected);
  oos::get(item, "val_date", dateval.result);
  UNIT_ASSERT_EQUAL(dateval.result, dateval.expected, "not expected result value");

  oos::set(item, "val_time", timeval.expected);
  oos::get(item, "val_time", timeval.result);
  UNIT_ASSERT_EQUAL(timeval.result, timeval.expected, "not expected result value");
}

void ObjectStoreTestUnit::test_structure()
{
  typedef ObjectItem<Item> object_item_t;
  typedef object_ptr<object_item_t> object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  
  object_item_t *oi = new object_item_t("object_item", 42);
  
  item_ptr iptr(new Item);
  oi->ptr(iptr);
  
  object_item_ptr optr = ostore_.insert(oi);

  UNIT_ASSERT_GREATER(optr.id(), 0UL, "object id must be greater zero");

  iptr = optr->ptr();

  UNIT_ASSERT_GREATER(iptr.id(), 0UL, "object id must be greater zero");
}

void ObjectStoreTestUnit::test_structure_cyclic()
{
  class cyclic : public oos::serializable
  {
  public:
    cyclic() {}
    cyclic(const std::string &n) : name(n) {}
    virtual ~cyclic() {}

    virtual void deserialize(oos::object_reader &r) {
      r.read("id", id);
      r.read("name", name);
      r.read("cycler", cycler);
    }
    virtual void serialize(oos::object_writer &w) const {
      w.write("id", id);
      w.write("name", name);
      w.write("cycler", cycler);
    }
    oos::identifier<unsigned long> id;
    std::string name;
    object_ptr<cyclic> cycler;
  };

  object_store ostore;
  ostore.prototypes().insert<cyclic>("cyclic");

  using cyclic_ptr = object_ptr<cyclic>;

  cyclic_ptr c1(new cyclic("c1"));
  cyclic_ptr c2(new cyclic("c2"));

  // cycle: c1 -> c2 -> c1
  c1->cycler = c2;
  c2->cycler = c1;

  cyclic_ptr cptr = ostore.insert(c1);

  UNIT_ASSERT_EQUAL(cptr.ref_count(), 0UL, "reference count must be zero");
  UNIT_ASSERT_EQUAL(cptr.ptr_count(), 1UL, "reference count must be one");

  UNIT_ASSERT_EQUAL(c2.ref_count(), 0UL, "reference count must be zero");
  UNIT_ASSERT_EQUAL(c2.ptr_count(), 1UL, "reference count must be one");
}

void ObjectStoreTestUnit::test_structure_container()
{
  object_store ostore;
  ostore.prototypes().insert<child>("cild");
  ostore.prototypes().insert<children_list>("cildren_list");

  using childrens_ptr = object_ptr<children_list>;

  childrens_ptr childrens(new children_list("ch1"));

  childrens->children.push_back(new child("heinz"));

  ostore.insert(childrens);

  object_ref<child> c1 = (*childrens->children.begin())->value();

  UNIT_ASSERT_GREATER(c1.id(), 0UL, "object store must be greater zero");
  UNIT_ASSERT_EQUAL(c1.ref_count(), 0UL, "reference count must be zero");
  UNIT_ASSERT_EQUAL(c1.ptr_count(), 1UL, "reference count must be one");

}

void ObjectStoreTestUnit::test_transient_optr()
{
  typedef object_ptr<Item> item_ptr;

  item_ptr item(new Item("item", 5));


  item_ptr item_copy = item;


  std::cout << item_copy->get_string() << "\n";

  item_copy.reset();

  std::cout << item->get_string() << "\n";
}

void ObjectStoreTestUnit::test_insert()
{
  UNIT_ASSERT_EXCEPTION(ostore_.insert((serializable *)0), object_exception, "serializable is null", "null shouldn't be insertable");

  ItemC *ic = new ItemC;
  UNIT_ASSERT_EXCEPTION(ostore_.insert(ic), object_exception, "couldn't insert serializable", "unknown serializable type shouldn't be insertable");
  delete ic;

  object_ptr<Item> item = ostore_.insert(new Item("test"));
  UNIT_ASSERT_NOT_NULL(item.ptr(), "internal pointer should not be zero");
  UNIT_ASSERT_TRUE(item->id() > 0, "id must be greater zero");
}

void ObjectStoreTestUnit::test_remove()
{
  typedef object_ptr<Item> item_ptr;

  item_ptr item;

  UNIT_ASSERT_EXCEPTION(ostore_.remove(item), object_exception, "serializable proxy is nullptr", "null shouldn't be removable");

  Item *i = new Item;
  item = i;

  UNIT_ASSERT_EXCEPTION(ostore_.remove(item), object_exception, "prototype node is nullptr", "transient serializable shouldn't be removable");
}

void ObjectStoreTestUnit::test_primary_key()
{
  typedef object_ptr<Item> item_ptr;

  item_ptr item;

  UNIT_ASSERT_FALSE(item.has_primary_key(), "item must not have a primary key");

  item = new Item("Test");

  UNIT_ASSERT_TRUE(item.has_primary_key(), "item must have a primary key");
}

void ObjectStoreTestUnit::test_to_many()
{
//  typedef object_ptr<employee> emp_ptr;
//  typedef object_ptr<department> dep_ptr;


}
