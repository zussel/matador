#include "ObjectStoreTestUnit.hpp"
#include "../Item.hpp"
#include "../entities.hpp"

#include "matador/object/object_expression.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/generic_access.hpp"

#include "matador/utils/algorithm.hpp"

#include "version.hpp"
#include <iostream>

using namespace matador;
using namespace std;

ObjectStoreTestUnit::ObjectStoreTestUnit()
  : unit_test("store", "ObjectStore Test Unit")
{
  add_test("version", std::bind(&ObjectStoreTestUnit::test_version, this), "test matador version");
  add_test("optr", std::bind(&ObjectStoreTestUnit::test_optr, this), "test optr behaviour");
  add_test("expression", std::bind(&ObjectStoreTestUnit::test_expression, this), "test object expressions");
  add_test("set", std::bind(&ObjectStoreTestUnit::test_set, this), "access object values via set interface");
  add_test("get", std::bind(&ObjectStoreTestUnit::test_get, this), "access object values via get interface");
  add_test("serializer", std::bind(&ObjectStoreTestUnit::test_serializer, this), "serializer test");
  add_test("identifier_serializer", std::bind(&ObjectStoreTestUnit::test_identifier_serializer, this), "identifier serializer test");
  add_test("reference_counter", std::bind(&ObjectStoreTestUnit::test_reference_counter, this), "reference counter test");
  add_test("simple", std::bind(&ObjectStoreTestUnit::test_simple_object, this), "create and delete one object");
  add_test("with_sub", std::bind(&ObjectStoreTestUnit::test_object_with_sub_object, this), "create and delete object with sub object");
  add_test("multiple_simple", std::bind(&ObjectStoreTestUnit::test_multiple_simple_objects, this), "create and delete multiple objects");
  add_test("multiple_object_with_sub", std::bind(&ObjectStoreTestUnit::test_multiple_object_with_sub_objects, this), "create and delete multiple objects with sub object");
  add_test("delete", std::bind(&ObjectStoreTestUnit::test_delete_object, this), "object deletion test");
  add_test("hierarchy", std::bind(&ObjectStoreTestUnit::test_hierarchy, this), "object hierarchy test");
  add_test("view", std::bind(&ObjectStoreTestUnit::test_view, this), "object view test");
//  add_test("clear_type", std::bind(&ObjectStoreTestUnit::test_clear_type, this), "object store clear type test");
  add_test("clear", std::bind(&ObjectStoreTestUnit::test_clear, this), "object store clear test");
  add_test("generic", std::bind(&ObjectStoreTestUnit::test_generic, this), "generic object access test");
  add_test("structure", std::bind(&ObjectStoreTestUnit::test_structure, this), "object transient structure test");
  add_test("structure_cyclic", std::bind(&ObjectStoreTestUnit::test_structure_cyclic, this), "object transient cyclic structure test");
  add_test("structure_container", std::bind(&ObjectStoreTestUnit::test_structure_container, this), "object transient container structure test");
  add_test("transient_optr", std::bind(&ObjectStoreTestUnit::test_transient_optr, this), "test transient object pointer");
  add_test("insert", std::bind(&ObjectStoreTestUnit::test_insert, this), "object insert test");
  add_test("remove", std::bind(&ObjectStoreTestUnit::test_remove, this), "object remove test");
  add_test("pk", std::bind(&ObjectStoreTestUnit::test_primary_key, this), "object proxy primary key test");
  add_test("has_many", std::bind(&ObjectStoreTestUnit::test_has_many, this), "has many test");
  add_test("has_many_to_many", std::bind(&ObjectStoreTestUnit::test_has_many_to_many, this), "has many to many test");
  add_test("belongs_to_one", std::bind(&ObjectStoreTestUnit::test_belongs_to_one, this), "test belongs to one behaviour");
  add_test("belongs_to_many", std::bind(&ObjectStoreTestUnit::test_belongs_to_many, this), "test belongs to many behaviour");
  add_test("observer", std::bind(&ObjectStoreTestUnit::test_observer, this), "test observer functionality");
  add_test("attach_has_many", std::bind(&ObjectStoreTestUnit::test_attach_has_many, this), "test attach has many");
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
  explicit test_pair(T exp) : expected(std::move(exp)) {}
  T expected;
  T result;
};

template <>
struct test_pair<char*> : public basic_test_pair
{
  explicit test_pair(const char exp[],size_t s)
    : expected(exp)
    , result(new char[s])
    , expected_size(strlen(exp))
    , size(s)
  {}
  ~test_pair() { delete [] result; }
  const char* expected;
  char* result;
  size_t expected_size;
  size_t size;
};

void ObjectStoreTestUnit::finalize()
{
  ostore_.clear(true);
}

struct item_counter : public std::unary_function<const object_ptr<ObjectItem<Item> >&, void>
{
  explicit item_counter(int &c) : count(c) {}
  
  void operator ()(const object_ptr<ObjectItem<Item> > &) { ++count; }
  int &count;
};

void ObjectStoreTestUnit::test_version()
{
  std::string version("0.6.0");
  
  UNIT_ASSERT_EQUAL(matador::version::str, version);

  UNIT_ASSERT_EQUAL(matador::version::major, 0);
  UNIT_ASSERT_EQUAL(matador::version::minor, 6);
  UNIT_ASSERT_EQUAL(matador::version::patch_level, 0);
}


void ObjectStoreTestUnit::test_optr()
{
  ostore_.attach<Item>("item");

  typedef object_ptr<Item> item_ptr;

  item_ptr item_null;

  UNIT_ASSERT_EXCEPTION(ostore_.insert(item_null), object_exception, "proxy is null");

  item_ptr item(new Item("Test"));

  UNIT_ASSERT_NULL(item.store());

  item = ostore_.insert(item);

  UNIT_ASSERT_NOT_NULL(item.store());
}

void ObjectStoreTestUnit::test_expression()
{
  ostore_.attach<Item>("item");
  ostore_.attach<ObjectItem<Item> >("object_item");
  ostore_.attach<ObjectItemList>("object_item_ptr_list");

  typedef object_ptr<ObjectItem<Item> > object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  typedef object_ptr<ObjectItemList> itemlist_ptr;

  itemlist_ptr itemlist = ostore_.insert(new ObjectItemList("object_ptr_list"));

  item_ptr ii;
  for (int i = 0; i < 10; ++i) {
    object_item_ptr oi = ostore_.insert(new ObjectItem<Item>("ObjectItem", i));
    ii = ostore_.insert(new Item("Item", i));
    oi->ptr(ii);
    itemlist->items.push_back(oi);
  }

  variable<int> x(make_var(&ObjectItem<Item>::get_int));
  variable<std::string> y(make_var(&ObjectItem<Item>::get_string));
  variable<object_ptr<Item> > u(make_var(&ObjectItem<Item>::ptr));

  object_view<ObjectItem<Item> > oview(ostore_);
  
  int count(0);
  for_each_if(oview.begin(), oview.end(), x >= 3 && x <= 7 && x != 5, item_counter(count));

  UNIT_ASSERT_EQUAL(count, 4);

  variable<int> z(make_var(&ObjectItem<Item>::get_int));

  std::unique_ptr<expression> exp(make_expression(z == 4));

  object_ptr<ObjectItem<Item>> optr = *itemlist->begin();

  (*exp)(optr);

#ifndef _MSC_VER
  ObjectItemList::iterator it = std::find_if(itemlist->begin(), itemlist->end(), z == 4);
  UNIT_ASSERT_FALSE(it == itemlist->end());
#endif

  object_view<ObjectItem<Item> >::iterator j = std::find_if(oview.begin(), oview.end(), 6 > x);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 1);

  j = std::find_if(oview.begin(), oview.end(), x > 6);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 7);

  j = std::find_if(oview.begin(), oview.end(), x < 6);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 1);

  j = std::find_if(oview.begin(), oview.end(), x == 6);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 6);

  j = std::find_if(oview.begin(), oview.end(), 6 == x);
  UNIT_ASSERT_EQUAL((*j)->get_int(), 6);

  j = std::find_if(oview.begin(), oview.end(), (6 == x) || (x < 4));
  UNIT_ASSERT_EQUAL((*j)->get_int(), 1);

  j = std::find_if(oview.begin(), oview.end(), u == ii);
  UNIT_ASSERT_EQUAL((*j)->ptr(), ii);

  j = std::find_if(oview.begin(), oview.end(), y == std::string("ObjectItem"));
  UNIT_ASSERT_EQUAL((*j)->get_string(), "ObjectItem");

  // try to find unknown object
  j = std::find_if(oview.begin(), oview.end(), y == std::string("Simple"));
  UNIT_ASSERT_TRUE(j == oview.end());

  j = std::find_if(oview.begin(), oview.end(), (x > 6) && (y == std::string("ObjectItem")));
  UNIT_ASSERT_EQUAL((*j)->get_int(), 7);
  UNIT_ASSERT_EQUAL((*j)->get_string(), "ObjectItem");
}

void ObjectStoreTestUnit::test_serializer()
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
  matador::varchar<64> str("The answer is 42");
  matador::date dt(15, 9, 1972);
  matador::time t(2008, 12, 27, 13, 6, 57, 4711);

  auto *item = new Item();
  
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

  UNIT_ASSERT_EQUAL(c, item->get_char());
  UNIT_ASSERT_EQUAL(f, item->get_float());
  UNIT_ASSERT_EQUAL(d, item->get_double());
  UNIT_ASSERT_EQUAL(s, item->get_short());
  UNIT_ASSERT_EQUAL(i, item->get_int());
  UNIT_ASSERT_EQUAL(l, item->get_long());
  UNIT_ASSERT_EQUAL(us, item->get_unsigned_short());
  UNIT_ASSERT_EQUAL(ui, item->get_unsigned_int());
  UNIT_ASSERT_EQUAL(ul, item->get_unsigned_long());
  UNIT_ASSERT_EQUAL(b, item->get_bool());
  UNIT_ASSERT_EQUAL(title, item->get_string());
  UNIT_ASSERT_EQUAL(str, item->get_varchar());
  UNIT_ASSERT_EQUAL(dt, item->get_date());
  UNIT_ASSERT_EQUAL(t, item->get_time());

  delete item;
}

void ObjectStoreTestUnit::test_identifier_serializer()
{
  byte_buffer buffer;
  basic_identifier_serializer serializer;

  {
    typedef identifier<unsigned long> t_ul_id;
    std::unique_ptr<t_ul_id> id(new t_ul_id(8UL));

    UNIT_ASSERT_EQUAL(id->value(), 8UL);

    serializer.serialize(*id, buffer);

    std::unique_ptr<t_ul_id> id2(new identifier<unsigned long>());

    serializer.deserialize(*id2, buffer);

    UNIT_ASSERT_EQUAL(id2->value(), 8UL);

    UNIT_ASSERT_EXCEPTION(serializer.deserialize(*id2, buffer), std::logic_error, "invalid identifier type");
  }

  {
    typedef identifier<std::string> t_str_id;
    std::unique_ptr<t_str_id> id(new identifier<std::string>("hallo"));

    UNIT_ASSERT_EQUAL(id->value(), "hallo");

    serializer.serialize(*id, buffer);

    std::unique_ptr<t_str_id> id2(new identifier<std::string>());

    serializer.deserialize(*id2, buffer);

    UNIT_ASSERT_EQUAL(id2->value(), "hallo");

    UNIT_ASSERT_EXCEPTION(serializer.deserialize(*id2, buffer), std::logic_error, "invalid identifier type");
  }
}

void ObjectStoreTestUnit::test_reference_counter()
{
  ostore_.attach<Item>("item");
  ostore_.attach<ObjectItem<Item> >("object_item");

  Item *i = new Item("Item", 7);
  
  typedef object_ptr<Item> item_ptr;
  typedef object_ptr<ObjectItem<Item> > object_item_ptr;
  
  item_ptr item = ostore_.insert(i);

  UNIT_ASSERT_EQUAL(item.reference_count(), 0UL);

  object_item_ptr object_item_1 = ostore_.insert(new ObjectItem<Item>());
  object_item_ptr object_item_2 = ostore_.insert(new ObjectItem<Item>());
  object_item_1->ptr(item);

  UNIT_ASSERT_EQUAL(item.reference_count(), 1UL);

  auto a1 = item;
  auto a2 = item;

  UNIT_ASSERT_EQUAL(item.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(a1.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(a2.reference_count(), 1UL);

  object_item_1->ref(a1);

  UNIT_ASSERT_EQUAL(item.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(a1.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(a2.reference_count(), 2UL);
}


void ObjectStoreTestUnit::test_set()
{
  matador::date dt(15, 9, 1972);
  matador::time t(2008, 12, 27, 13, 6, 57, 4711);
  matador::varchar<64> varstr("The answer is 42");
  std::string str("tiger");

  Item i("item", 4711);
  
  matador::set(i, "val_char", 'f');
  matador::set(i, "val_short", -2);
  matador::set(i, "val_int", -1);
  matador::set(i, "val_long", -7);
  matador::set(i, "val_unsigned_short", 2);
  matador::set(i, "val_unsigned_int", 1);
  matador::set(i, "val_unsigned_long", 7);
  matador::set(i, "val_bool", false);
  matador::set(i, "val_float", 0.456);
  matador::set(i, "val_double", 3.1415);
  matador::set(i, "val_cstr", "tiger");
  matador::set(i, "val_string", str);
  matador::set(i, "val_varchar", varstr);
  matador::set(i, "val_date", dt);
  matador::set(i, "val_time", t);

  UNIT_ASSERT_EQUAL('f', i.get_char());
  UNIT_ASSERT_EQUAL(-2, i.get_short());
  UNIT_ASSERT_EQUAL(-1, i.get_int());
  UNIT_ASSERT_EQUAL(-7L, i.get_long());
  UNIT_ASSERT_EQUAL(2, i.get_unsigned_short());
  UNIT_ASSERT_EQUAL(1U, i.get_unsigned_int());
  UNIT_ASSERT_EQUAL(7UL, i.get_unsigned_long());
  UNIT_ASSERT_EQUAL(7UL, i.get_unsigned_long());
  UNIT_ASSERT_EQUAL(false, i.get_bool());
  UNIT_ASSERT_EQUAL(0.456f, i.get_float());
  UNIT_ASSERT_EQUAL(3.1415, i.get_double());
  UNIT_ASSERT_EQUAL("tiger", i.get_string());
  UNIT_ASSERT_EQUAL(str, i.get_string());
  UNIT_ASSERT_EQUAL(varstr, i.get_varchar());
  UNIT_ASSERT_EQUAL(dt, i.get_date());
  UNIT_ASSERT_EQUAL(t, i.get_time());

  matador::set(i, "val_string", "lion");
  UNIT_ASSERT_EQUAL("lion", i.get_string());

//  auto vc = matador::varchar<16>("elefant");
//  matador::set(i, "val_string", vc);
//  UNIT_ASSERT_EQUAL("elefant", i.get_string());
}

void ObjectStoreTestUnit::test_get()
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
  test_pair<matador::varchar<64> > varstr("The answer is 42");
  test_pair<matador::date> dateval(matador::date("29.4.1972"));
  test_pair<matador::time > timeval(matador::time(2015, 10, 16, 8, 54, 32, 123));

  Item item;

  item.set_char(c.expected);
  item.set_float(f.expected);
  item.set_double(d.expected);
  item.set_short(s.expected);
  item.set_int(i.expected);
  item.set_long(l.expected);
  item.set_unsigned_short(us.expected);
  item.set_unsigned_int(ui.expected);
  item.set_unsigned_long(ul.expected);
  item.set_bool(b.expected);
  item.set_string(str.expected);
  item.set_varchar(varstr.expected);
  item.set_cstr(cstr.expected, cstr.size);
  item.set_date(dateval.expected);
  item.set_time(timeval.expected);

  matador::get(item, "val_char", c.result);
  UNIT_ASSERT_EQUAL(c.result, c.expected);
  matador::get(item, "val_short", s.result);
  UNIT_ASSERT_EQUAL(s.result, s.expected);
  matador::get(item, "val_int", i.result);
  UNIT_ASSERT_EQUAL(i.result, i.expected);
  matador::get(item, "val_long", l.result);
  UNIT_ASSERT_EQUAL(l.result, l.expected);
  matador::get(item, "val_unsigned_short", us.result);
  UNIT_ASSERT_EQUAL(us.result, us.expected);
  matador::get(item, "val_unsigned_int", ui.result);
  UNIT_ASSERT_EQUAL(ui.result, ui.expected);
  matador::get(item, "val_unsigned_long", ul.result);
  UNIT_ASSERT_EQUAL(ul.result, ul.expected);
  matador::get(item, "val_float", f.result);
  UNIT_ASSERT_EQUAL(f.result, f.expected);
  matador::get(item, "val_double", d.result);
  UNIT_ASSERT_EQUAL(d.result, d.expected);
  matador::get(item, "val_bool", b.result);
  UNIT_ASSERT_EQUAL(b.result, b.expected);
  matador::get(item, "val_string", str.result);
  UNIT_ASSERT_EQUAL(str.result, str.expected);
  matador::get(item, "val_varchar", varstr.result);
  UNIT_ASSERT_EQUAL(varstr.result, varstr.expected);
  matador::get(item, "val_cstr", cstr.result, cstr.size);
  UNIT_ASSERT_EQUAL(cstr.result, cstr.expected);
  matador::get(item, "val_date", dateval.result);
  UNIT_ASSERT_EQUAL(dateval.result, dateval.expected);
  matador::get(item, "val_time", timeval.result);
  UNIT_ASSERT_EQUAL(timeval.result, timeval.expected);
}

void ObjectStoreTestUnit::test_simple_object()
{
  ostore_.attach<Item>("item");

  auto *a = ostore_.create<Item>();
  
  UNIT_ASSERT_NOT_NULL(a);
  
  typedef object_ptr<Item> item_ptr;
  
  item_ptr simple = ostore_.insert(a);
  
  UNIT_ASSERT_NOT_NULL(simple.get());
  
  UNIT_ASSERT_TRUE(ostore_.is_removable(simple));
  
  ostore_.remove(simple);
}

void ObjectStoreTestUnit::test_object_with_sub_object()
{
  ostore_.attach<Item>("item");
  ostore_.attach<ObjectItem<Item> >("object_item");

  auto *s = ostore_.create<ObjectItem<Item>>();
  
  UNIT_ASSERT_NOT_NULL(s);
  
  typedef object_ptr<ObjectItem<Item> > obj_item_ptr;
  
  obj_item_ptr ows = ostore_.insert(s);
  
  UNIT_ASSERT_NOT_NULL(ows.get());
  
  // check if sub object exists
  object_ptr<Item> simple = ows->ptr();
  
  UNIT_ASSERT_NULL(simple.get());
  
  UNIT_ASSERT_TRUE(ostore_.is_removable(ows));
  
  ostore_.remove(ows);
}

void ObjectStoreTestUnit::test_multiple_simple_objects()
{
  ostore_.attach<Item>("item");

  typedef object_ptr<Item> item_ptr;

  size_t elem_size = 10;
  // create 10 objects
  for (size_t i = 0; i < elem_size; ++i) {
    auto *a = ostore_.create<Item>();
    
    UNIT_ASSERT_NOT_NULL(a);
    
    item_ptr simple = ostore_.insert(a);
  }

  typedef object_view<Item> simple_view_t;
  simple_view_t simple_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, simple_view.size());
}

void ObjectStoreTestUnit::test_multiple_object_with_sub_objects()
{
  ostore_.attach<Item>("item");
  ostore_.attach<ObjectItem<Item> >("object_item");

  typedef object_ptr<ObjectItem<Item> > ows_ptr;
    
  // create 10 objects
  size_t elem_size = 10;
  for (size_t i = 0; i < elem_size; ++i) {
    auto *s = ostore_.create<ObjectItem<Item>>();

    UNIT_ASSERT_NOT_NULL(s);
    
    ows_ptr ows = ostore_.insert(s);
  }

  typedef object_view<ObjectItem<Item> > withsub_view_t;
  withsub_view_t withsub_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, withsub_view.size());
}

void ObjectStoreTestUnit::test_delete_object()
{
//  std::cout << "\n";
  ostore_.attach<Item>("item");
  ostore_.attach<ObjectItem<Item> >("object_item");

  typedef ObjectItem<Item> TestItem;
  typedef object_ptr<TestItem> test_item_ptr;
  typedef object_ptr<Item> item_ptr;

  item_ptr item = ostore_.insert(new Item("item 1"));

  auto *ti = new TestItem;
  ti->ref(item);
  
  test_item_ptr testitem = ostore_.insert(ti);

  UNIT_ASSERT_EQUAL(item.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(testitem.reference_count(), 0UL);

  typedef object_view<Item> item_view_t;
  item_view_t item_view(ostore_);

  UNIT_ASSERT_FALSE(ostore_.is_removable(item));

//  std::cout << "remove item from store (false)\n";
  UNIT_ASSERT_EXCEPTION(ostore_.remove(item), object_exception, "object is not removable");

  UNIT_ASSERT_TRUE(ostore_.is_removable(testitem));

//  std::cout << "remove testitem from store\n";
  ostore_.remove(testitem);

  UNIT_ASSERT_TRUE(ostore_.is_removable(item));

//  std::cout << "remove item from store (true)\n";
  ostore_.remove(item);
}

void ObjectStoreTestUnit::test_hierarchy()
{
  ostore_.attach<Item>("item");
  ostore_.attach<ItemA, Item>("ITEM_A");
  ostore_.attach<ItemB, Item>("ITEM_B");
  ostore_.attach<ItemC, Item>("ITEM_C");
  
  /* Insert 5 object of each item
   * object type
   */

  for (int i = 0; i < 5; ++i) {
    std::stringstream str;
    str << "item " << i;
    ostore_.insert(new Item(str.str()));
    str.str("");
    str << "item a " << i;
    auto *ia = new ItemA;
    ia->set_string(str.str());
    ostore_.insert(ia);
    str.str("");
    str << "item b " << i;
    auto *ib = new ItemB;
    ib->set_string(str.str());
    ostore_.insert(ib);
    str.str("");
    str << "item c " << i;
    auto *ic = new ItemC;
    ic->set_string(str.str());
    ostore_.insert(ic);
  }
  
  typedef object_view<Item> item_view_t;

  item_view_t item_view(ostore_);

  /************************************
   * 
   * list all items with sub items
   *
   ************************************/

  size_t max = 20;
  UNIT_ASSERT_EQUAL(item_view.size(), max);
  
  item_view_t::const_iterator first = item_view.begin();
  item_view_t::const_iterator last = item_view.end();

  int count = 0;
  while (first != last) {
    UNIT_ASSERT_LESS(count, 20);
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
  UNIT_ASSERT_EQUAL(item_view.size(), max);
  
  first = item_view.begin();
  last = item_view.end();
  
  count = 0;
  while (first != last) {
    UNIT_ASSERT_LESS(count, 5);
    ++first;
    ++count;
  }
  
  // Todo: check deletion
  
  // Todo: check sub item view (a,b,c)
  typedef object_view<ItemA> item_a_view_t;

  item_a_view_t item_a_view(ostore_);

  UNIT_ASSERT_EQUAL(item_a_view.size(), max);
  
  item_a_view_t::const_iterator afirst = item_a_view.begin();
  item_a_view_t::const_iterator alast = item_a_view.end();

  count = 0;
  while (afirst != alast) {
    UNIT_ASSERT_LESS(count, 5);
    ++afirst;
    ++count;
  }
}

void ObjectStoreTestUnit::test_view()
{
  ostore_.attach<Item>("item");

  for (int i = 0; i < 10; ++i) {
    std::stringstream str;
    str << "Item " << i+1;
    ostore_.insert(new Item(str.str(), i+1));
  }

  typedef object_ptr<Item> item_ptr;
  typedef object_view<Item> item_view_t;

  item_view_t iview(ostore_);

  UNIT_ASSERT_EQUAL((int)iview.size(), 10);
  UNIT_ASSERT_FALSE(iview.empty());

  item_ptr item = iview.front();

  UNIT_ASSERT_GREATER(item->id(), 0UL);

  item = iview.back();
  
  UNIT_ASSERT_GREATER(item->id(), 0UL);
}

//void ObjectStoreTestUnit::test_clear_type()
//{
//  ostore_.attach<Item>("item");
//
//  for (int i = 0; i < 10; ++i) {
//    std::stringstream str;
//    str << "Item " << i+1;
//    ostore_.insert(new Item(str.str(), i+1));
//  }
//
//  typedef object_view<Item> item_view_t;
//
//  item_view_t iview(ostore_);
//
//  UNIT_ASSERT_EQUAL((int)iview.size(), 10);
//  UNIT_ASSERT_FALSE(iview.empty());
//  UNIT_ASSERT_FALSE(ostore_.empty());
//
//  ostore_.clear("item");
//
//  UNIT_ASSERT_TRUE(ostore_.empty());
//  UNIT_ASSERT_EQUAL(iview.size(), 0UL);
//  UNIT_ASSERT_TRUE(iview.empty());
//
//  prototype_iterator first = ostore_.begin();
//  prototype_iterator last = ostore_.end();
//
//  UNIT_ASSERT_FALSE(first == last);
//
//  ostore_.clear(true);
//
//  first = ostore_.begin();
//  last = ostore_.end();
//
//  UNIT_ASSERT_TRUE(first == last);
//}

void ObjectStoreTestUnit::test_clear()
{
  ostore_.attach<Item>("item");

  for (int i = 0; i < 10; ++i) {
    std::stringstream str;
    str << "Item " << i+1;
    ostore_.insert(new Item(str.str(), i+1));
  }

  typedef object_view<Item> item_view_t;

  item_view_t iview(ostore_);

  UNIT_ASSERT_EQUAL((int)iview.size(), 10);
  UNIT_ASSERT_FALSE(iview.empty());
  UNIT_ASSERT_FALSE(ostore_.empty());

  ostore_.clear();

  UNIT_ASSERT_TRUE(ostore_.empty());
  UNIT_ASSERT_EQUAL(iview.size(), 0UL);
  UNIT_ASSERT_TRUE(iview.empty());

  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();

  UNIT_ASSERT_FALSE(first == last);

  ostore_.clear(true);

  first = ostore_.begin();
  last = ostore_.end();

  UNIT_ASSERT_TRUE(first == last);
}

void ObjectStoreTestUnit::test_generic()
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
  test_pair<matador::varchar<64> > varstr("The answer is 42");
  test_pair<matador::date> dateval(matador::date("29.4.1972"));
  test_pair<matador::time > timeval(matador::time(2015, 10, 16, 8, 54, 32, 123));

  std::unique_ptr<Item> item(new Item());

  matador::set(*item, "val_char", c.expected);
  matador::get(*item, "val_char", c.result);
  UNIT_ASSERT_EQUAL(c.result, c.expected);

  matador::set(*item, "val_short", s.expected);
  matador::get(*item, "val_short", s.result);
  UNIT_ASSERT_EQUAL(s.result, s.expected);
  matador::get(*item, "val_short", s.str_result);
  UNIT_ASSERT_EQUAL(s.str_result, "-42");

  matador::set(*item, "val_int", i.expected);
  matador::get(*item, "val_int", i.result);
  UNIT_ASSERT_EQUAL(i.result, i.expected);

  matador::set(*item, "val_long", l.expected);
  matador::get(*item, "val_long", l.result);
  UNIT_ASSERT_EQUAL(l.result, l.expected);

  matador::set(*item, "val_unsigned_short", us.expected);
  matador::get(*item, "val_unsigned_short", us.result);
  UNIT_ASSERT_EQUAL(us.result, us.expected);

  matador::set(*item, "val_unsigned_int", ui.expected);
  matador::get(*item, "val_unsigned_int", ui.result);
  UNIT_ASSERT_EQUAL(ui.result, ui.expected);

  matador::set(*item, "val_unsigned_long", ul.expected);
  matador::get(*item, "val_unsigned_long", ul.result);
  UNIT_ASSERT_EQUAL(ul.result, ul.expected);

  matador::set(*item, "val_bool", b.expected);
  matador::get(*item, "val_bool", b.result);
  UNIT_ASSERT_EQUAL(b.result, b.expected);

  matador::set(*item, "val_cstr", cstr.expected);
  matador::get(*item, "val_cstr", cstr.result, cstr.size);
  UNIT_ASSERT_EQUAL(cstr.result, cstr.expected);

  matador::set(*item, "val_string", str.expected);
  matador::get(*item, "val_string", str.result);
  UNIT_ASSERT_EQUAL(str.result, str.expected);

  matador::set(*item, "val_varchar", varstr.expected);
  matador::get(*item, "val_varchar", varstr.result);
  UNIT_ASSERT_EQUAL(varstr.result, varstr.expected);

  matador::set(*item, "val_float", f.expected);
  matador::get(*item, "val_float", f.result);
  UNIT_ASSERT_EQUAL(f.result, f.expected);
  /* get float value into string
   * with precision 2
   */
  matador::get(*item, "val_float", f.str_result, 2);
  UNIT_ASSERT_EQUAL(f.str_result, "1.55");

  matador::set(*item, "val_double", d.expected);
  matador::get(*item, "val_double", d.result);
  UNIT_ASSERT_EQUAL(d.result, d.expected);
  /* get double value into string
   * with precision 3
   */
  matador::get(*item, "val_double", d.str_result, 3);
  UNIT_ASSERT_EQUAL(d.str_result, "123.558");

  matador::get(*item, "val_int", str.result);
  UNIT_ASSERT_EQUAL(str.result, "-98765");

  matador::set(*item, "val_date", dateval.expected);
  matador::get(*item, "val_date", dateval.result);
  UNIT_ASSERT_EQUAL(dateval.result, dateval.expected);

  matador::set(*item, "val_time", timeval.expected);
  matador::get(*item, "val_time", timeval.result);
  UNIT_ASSERT_EQUAL(timeval.result, timeval.expected);

  master m1("master 1");

  object_ptr<child> c1(new child("child 1"));
  object_ptr<child> child_result;

  matador::set(m1, "child", c1);
  matador::get(m1, "child", child_result);
  UNIT_ASSERT_EQUAL(c1->name, child_result->name);

  matador::set(m1, "child", object_ptr<child>());
  matador::get(m1, "child", child_result);
  UNIT_ASSERT_TRUE(child_result.get() == nullptr);

  children_vector cv("children vector");

  UNIT_ASSERT_TRUE(cv.children.empty());
  matador::append(cv, "children", c1);
  UNIT_ASSERT_FALSE(cv.children.empty());
  matador::remove(cv, "children", c1);
  UNIT_ASSERT_TRUE(cv.children.empty());
  // Todo: matador::remove, matador::begin, matador::end, matador::size, matador::empty for generic access has_many
  // see: https://tartanllama.github.io/c++/2017/01/03/deduction-on-the-left/ for begin and end
//
//  auto i = matador::begin(cv);
//  auto i = matador::end(cv);
//
//  auto size = matador::size(cv);
//  bool is_empty = matador::empty(cv);
}

void ObjectStoreTestUnit::test_structure()
{
  ostore_.attach<Item>("item");
  ostore_.attach<ObjectItem<Item> >("object_item");

  typedef ObjectItem<Item> object_item_t;
  typedef object_ptr<object_item_t> object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  
  object_item_t *oi = new object_item_t("object_item", 42);
  
  item_ptr iptr(new Item);
  oi->ptr(iptr);
  
  object_item_ptr optr = ostore_.insert(oi);

  UNIT_ASSERT_GREATER(optr.id(), 0UL);

  iptr = optr->ptr();

  UNIT_ASSERT_GREATER(iptr.id(), 0UL);
}

class cyclic
{
public:
  cyclic() = default;
  explicit cyclic(std::string n) : name(std::move(n)) {}
  ~cyclic() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &w) {
    w.serialize("id", id);
    w.serialize("name", name);
    w.serialize("cycler", cycler, cascade_type::ALL);
  }
  matador::identifier<unsigned long> id;
  std::string name;
  has_one<cyclic> cycler;
};

void ObjectStoreTestUnit::test_structure_cyclic()
{
  object_store ostore;
  ostore.attach<cyclic>("cyclic");

  using cyclic_ptr = object_ptr<cyclic>;

  cyclic_ptr c1(new cyclic("c1"));
  cyclic_ptr c2(new cyclic("c2"));
  cyclic_ptr c3(new cyclic("c3"));

  UNIT_ASSERT_EQUAL(c1.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(c2.reference_count(), 0UL);

  // cycle: c1 -> c2 -> c1
  c1->cycler = c2;

  UNIT_ASSERT_EQUAL(c1.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(c2.reference_count(), 0UL);

  c2->cycler = c1;

  UNIT_ASSERT_EQUAL(c1.reference_count(), 0UL);
  UNIT_ASSERT_EQUAL(c2.reference_count(), 0UL);

  c1 = ostore.insert(c1);

  UNIT_ASSERT_EQUAL(c1.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(c2.reference_count(), 1UL);

  c3->cycler = c1;

  UNIT_ASSERT_EQUAL(c1.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(c2.reference_count(), 1UL);

  c3 = ostore.insert(c3);

  UNIT_ASSERT_EQUAL(c1.reference_count(), 2UL);
  UNIT_ASSERT_EQUAL(c2.reference_count(), 1UL);
  UNIT_ASSERT_EQUAL(c3.reference_count(), 0UL);
}

void ObjectStoreTestUnit::test_structure_container()
{
  object_store ostore;
  ostore.attach<child>("child");
  ostore.attach<children_vector>("children_vector");

  using childrens_ptr = object_ptr<children_vector>;

  childrens_ptr childrens(new children_vector("ch1"));

  childrens->children.push_back(new child("heinz"));

  ostore.insert(childrens);

  object_ptr<child> c1 = childrens->children.front();

  UNIT_ASSERT_GREATER(c1.id(), 0UL);
  UNIT_ASSERT_EQUAL(c1.reference_count(), 1UL);
}

void ObjectStoreTestUnit::test_transient_optr()
{
  ostore_.attach<Item>("item");

  typedef object_ptr<Item> item_ptr;

  item_ptr item(new Item("item", 5));

  item_ptr item_copy = item;

  item_copy.reset(nullptr, cascade_type::NONE);
}

void ObjectStoreTestUnit::test_insert()
{
  ostore_.attach<Item>("item");

  UNIT_ASSERT_EXCEPTION(ostore_.insert((Item *)nullptr), object_exception, "object is null");

  std::unique_ptr<ItemC> ic(new ItemC);
  UNIT_ASSERT_EXCEPTION(ostore_.insert(ic.get()), object_exception, "couldn't find object type");

  object_ptr<Item> item = ostore_.insert(new Item("test"));
  UNIT_ASSERT_NOT_NULL(item.ptr());
  UNIT_ASSERT_TRUE(item->id() > 0);
}

void ObjectStoreTestUnit::test_remove()
{
  ostore_.attach<Item>("item");

  typedef object_ptr<Item> item_ptr;

  item_ptr item;

  UNIT_ASSERT_EXCEPTION(ostore_.remove(item), object_exception, "object proxy is nullptr");

  auto *i = new Item;
  item = i;

  UNIT_ASSERT_EXCEPTION(ostore_.remove(item), object_exception, "prototype node is nullptr");
}

void ObjectStoreTestUnit::test_primary_key()
{
  typedef object_ptr<Item> item_ptr;

  item_ptr item;

  UNIT_ASSERT_FALSE(item.has_primary_key());

  item = new Item("Test");

  UNIT_ASSERT_TRUE(item.has_primary_key());
}

void ObjectStoreTestUnit::test_has_many()
{
  ostore_.attach<book>("book");
  ostore_.attach<book_list>("book_list");

  auto prototype = ostore_.find("books");

  UNIT_ASSERT_EQUAL("books", prototype->type());
  UNIT_ASSERT_FALSE(prototype->has_primary_key());

  ostore_.insert(new book_list);
}

void ObjectStoreTestUnit::test_has_many_to_many()
{
  ostore_.attach<person>("person");
  ostore_.attach<student, person>("student");
  ostore_.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, ostore_.size());

  auto george = ostore_.insert(new student("george"));
  auto jane = ostore_.insert(new student("jane"));
  auto algebra = ostore_.insert(new course("algebra"));
  auto art = ostore_.insert(new course("art"));

  UNIT_ASSERT_TRUE(george->courses.empty());
  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_TRUE(algebra->students.empty());
  UNIT_ASSERT_TRUE(art->students.empty());

  art->students.push_back(jane);

  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.front()->name(), jane->name());
  UNIT_ASSERT_FALSE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->courses.front()->title, art->title);

  jane->courses.erase(jane->courses.begin());

  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_TRUE(art->students.empty());

  george->courses.push_back(algebra);

  UNIT_ASSERT_FALSE(algebra->students.empty());
  UNIT_ASSERT_EQUAL(algebra->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(algebra->students.front()->name(), george->name());
  UNIT_ASSERT_FALSE(george->courses.empty());
  UNIT_ASSERT_EQUAL(george->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(george->courses.front()->title, algebra->title);

  algebra->students.clear();

  UNIT_ASSERT_TRUE(george->courses.empty());
  UNIT_ASSERT_TRUE(algebra->students.empty());
}

void ObjectStoreTestUnit::test_belongs_to_one()
{
  ostore_.attach<person>("person");
  ostore_.attach<address>("address");
  ostore_.attach<citizen, person>("citizen");

  auto george = ostore_.insert(new citizen("george"));
  auto home = ostore_.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(george->address_.empty());
  UNIT_ASSERT_TRUE(home->citizen_.empty());

//  george->address_ = home;
  home->citizen_ = george;

  UNIT_ASSERT_FALSE(george->address_.empty());
  UNIT_ASSERT_FALSE(home->citizen_.empty());

  george->address_.clear();

  UNIT_ASSERT_TRUE(george->address_.empty());
  UNIT_ASSERT_TRUE(home->citizen_.empty());

  george->address_ = home;

  UNIT_ASSERT_FALSE(george->address_.empty());
  UNIT_ASSERT_FALSE(home->citizen_.empty());
}

void ObjectStoreTestUnit::test_belongs_to_many()
{
  ostore_.attach<person>("person");
  ostore_.attach<department>("department");
  ostore_.attach<employee, person>("employee");

  // expected prototypes
  // person, employee and department

  auto george = ostore_.insert(new employee("george"));
  auto jane = ostore_.insert(new employee("jane"));
  auto dep = ostore_.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(dep->employees.empty());
  UNIT_ASSERT_TRUE(george->dep().empty());
  UNIT_ASSERT_TRUE(jane->dep().empty());

  // department is automatically set
  dep->employees.push_back(george);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "george");
  UNIT_ASSERT_FALSE(george->dep().empty());
  UNIT_ASSERT_EQUAL(george->dep()->name, dep->name);

  // jane is automatically added to deps employee list
  jane->dep(dep);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 2UL);

  // remove george
  auto i = dep->employees.begin();
  i = dep->employees.erase(i);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL);
  UNIT_ASSERT_TRUE(george->dep().empty());
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "jane");

  jane->department_.clear();

  UNIT_ASSERT_TRUE(dep->employees.empty());
}

struct basic_logger
{
  static std::vector<std::string> nodes;
};

std::vector<std::string> basic_logger::nodes = std::vector<std::string>();

template < class T >
struct logger : public object_store_observer<T>, public basic_logger
{
  logger() = default;

  template < class V >
  explicit logger(const logger<V> *) {}

  void on_attach(prototype_node &node, T &) override
  {
    nodes.emplace_back(node.type());
  }

  void on_detach(prototype_node &node, T &) override
  {
    nodes.emplace_back(node.type());
  }

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}
};

void ObjectStoreTestUnit::test_observer()
{
  ostore_.attach<person>("person", { new logger<person> });
  ostore_.attach<employee, person>("employee", { new logger<employee> });
  ostore_.attach<department>("department", { new logger<department> });
  ostore_.attach<book>("book", { new logger<book> });
  ostore_.attach<book_list>("book_list", { new logger<book_list> });

  std::vector<std::string> result({
    "person",
    "employee",
    "department",
    "book",
    "books",
    "book_list"
  });

  UNIT_ASSERT_TRUE(basic_logger::nodes == result);

  basic_logger::nodes.clear();

  ostore_.clear(true);

  ostore_.attach<person>("person", { new logger<person> });
  ostore_.attach<department>("department", { new logger<department> });
  ostore_.attach<employee, person>("employee", { new logger<employee> });
  ostore_.attach<book_list>("book_list", { new logger<book_list> });
  ostore_.attach<book>("book", { new logger<book> });

  result = {
    "person",
    "employee",
    "department",
    "employee",
    "employee",
    "books",
    "book_list",
    "book"
  };

  UNIT_ASSERT_TRUE(basic_logger::nodes == result);
}

typedef has_many_to_many_item<course, student> course_student_item;

void ObjectStoreTestUnit::test_attach_has_many()
{
  ostore_.attach<person>("person");
  ostore_.attach<student, person>("student");
  ostore_.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, ostore_.size());

  std::unique_ptr<course_student_item> item1(ostore_.create<course_student_item>());

  UNIT_ASSERT_NOT_NULL(item1.get());

  auto node = ostore_.find("student_course");

  UNIT_ASSERT_FALSE(node == ostore_.end());

  node = ostore_.find<course_student_item>();

  UNIT_ASSERT_FALSE(node == ostore_.end());
}
