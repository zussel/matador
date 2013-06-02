#include "ObjectStoreTestUnit.hpp"
#include "../Item.hpp"

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
  add_test("set", std::tr1::bind(&ObjectStoreTestUnit::set_test, this), "access object values via set interface");
  add_test("get", std::tr1::bind(&ObjectStoreTestUnit::get_test, this), "access object values via get interface");
  add_test("serializer", std::tr1::bind(&ObjectStoreTestUnit::serializer, this), "serializer test");
  add_test("ref_ptr_counter", std::tr1::bind(&ObjectStoreTestUnit::ref_ptr_counter, this), "ref and ptr counter test");
  add_test("simple", std::tr1::bind(&ObjectStoreTestUnit::simple_object, this), "create and delete one object");
  add_test("with_sub", std::tr1::bind(&ObjectStoreTestUnit::object_with_sub_object, this), "create and delete object with sub object");
  add_test("multiple_simple", std::tr1::bind(&ObjectStoreTestUnit::multiple_simple_objects, this), "create and delete multiple objects");
  add_test("multiple_object_with_sub", std::tr1::bind(&ObjectStoreTestUnit::multiple_object_with_sub_objects, this), "create and delete multiple objects with sub object");
  add_test("delete", std::tr1::bind(&ObjectStoreTestUnit::delete_object, this), "object deletion test");
  add_test("sub_delete", std::tr1::bind(&ObjectStoreTestUnit::sub_delete, this), "create and delete multiple objects with sub object");
  add_test("hierarchy", std::tr1::bind(&ObjectStoreTestUnit::hierarchy, this), "object hierarchy test");
  add_test("view", std::tr1::bind(&ObjectStoreTestUnit::view_test, this), "object view test");
  add_test("clear", std::tr1::bind(&ObjectStoreTestUnit::clear_test, this), "object store clear test");
  add_test("generic", std::tr1::bind(&ObjectStoreTestUnit::generic_test, this), "generic object access test");
  add_test("structure", std::tr1::bind(&ObjectStoreTestUnit::test_structure, this), "object structure test");
}

ObjectStoreTestUnit::~ObjectStoreTestUnit()
{}

class ObjectItemPtrList : public List<oos::object_ptr<ObjectItem<Item> > >
{
public:
  ObjectItemPtrList() : List<oos::object_ptr<ObjectItem<Item> > >("object_ptr_list") {}
  virtual ~ObjectItemPtrList() {}
};

void
ObjectStoreTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("ITEM");
  ostore_.insert_prototype<ObjectItem<Item> >("OBJECT_ITEM");
  ostore_.insert_prototype<ItemPtrList>("ITEM_PTR_LIST");
//  ostore_.insert_prototype<ItemPtrList::item_type>("ITEM_PTR");
  ostore_.insert_prototype<ObjectItemPtrList>("OBJECT_ITEM_PTR_LIST");
//  ostore_.insert_prototype<ObjectItemPtrList::item_type>("OBJECT_ITEM_PTR");
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
ObjectStoreTestUnit::expression_test()
{
  typedef object_ptr<ObjectItem<Item> > object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  typedef object_ptr<ObjectItemPtrList> itemlist_ptr;

  itemlist_ptr itemlist = ostore_.insert(new ObjectItemPtrList);

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


  variable<int> k(make_var(&ObjectItem<Item>::ptr, &Item::get_int));

  variable<int> z(make_var(&ObjectItemType::value, &ObjectItem<Item>::get_int));

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
  
  object_serializer serializer;
 
  byte_buffer buffer;
  serializer.serialize(item, buffer);
  
  delete item;
  
  item = new Item();
  
  serializer.deserialize(item, buffer, &ostore_);

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
ObjectStoreTestUnit::set_test()
{
  Item *i = new Item("item", 4711);
  
  std::string one("1");
  i->set("val_int", one);
  delete i;
}

void
ObjectStoreTestUnit::get_test()
{
}

void
ObjectStoreTestUnit::simple_object()
{
  object *o = ostore_.create("ITEM");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ITEM>");
  
  Item *a = dynamic_cast<Item*>(o);
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to Item");
  
  typedef object_ptr<Item> item_ptr;
  
  item_ptr simple = ostore_.insert(a);
  
  UNIT_ASSERT_NOT_NULL(simple.get(), "item object insertion failed");
  
  UNIT_ASSERT_TRUE(ostore_.is_removable(simple), "deletion of item object failed");
  
  ostore_.remove(simple);
}

void
ObjectStoreTestUnit::object_with_sub_object()
{
  object *o = ostore_.create("OBJECT_ITEM");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectItem>");
  
  ObjectItem<Item> *s = dynamic_cast<ObjectItem<Item>*>(o);
  
  UNIT_ASSERT_NOT_NULL(s, "couldn't cast object to ObjectItem<Item>");
  
  typedef object_ptr<ObjectItem<Item> > obj_item_ptr;
  
  obj_item_ptr ows = ostore_.insert(s);
  
  UNIT_ASSERT_NOT_NULL(ows.get(), "object item object insertion failed");
  
  // check if sub object exists
  object_ptr<Item> simple = ows->ptr();
  
  UNIT_ASSERT_NOT_NULL(simple.get(), "item object creation failed");
  
  UNIT_ASSERT_TRUE(ostore_.is_removable(ows), "deletion of object item failed");
  
  ostore_.remove(ows);
}

void
ObjectStoreTestUnit::multiple_simple_objects()
{
  typedef object_ptr<Item> item_ptr;

  size_t elem_size = 10000;
  // create 1000 objects
  for (size_t i = 0; i < elem_size; ++i) {
    object *o = ostore_.create("ITEM");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <Item>");
    
    Item *a = dynamic_cast<Item*>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to Item");
    
    item_ptr simple = ostore_.insert(a);
  }

  typedef object_view<Item> simple_view_t;
  simple_view_t simple_view(ostore_);

  UNIT_ASSERT_EQUAL(elem_size, simple_view.size(), "expected size of view isn't 10000");
}

void
ObjectStoreTestUnit::multiple_object_with_sub_objects()
{
  typedef object_ptr<ObjectItem<Item> > ows_ptr;
    
  // create 1000 objects
  size_t elem_size = 1000;
  for (size_t i = 0; i < elem_size; ++i) {
    object *o = ostore_.create("OBJECT_ITEM");
    
    UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ObjectItem<Item> >");
    
    ObjectItem<Item>  *a = dynamic_cast<ObjectItem<Item> *>(o);
    
    UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to ObjectItem<Item> ");
    
    ows_ptr ows = ostore_.insert(a);
  }

  typedef object_view<ObjectItem<Item> > withsub_view_t;
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
ObjectStoreTestUnit::sub_delete()
{
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
  UNIT_ASSERT_FALSE((int)iview.empty(), "item view is not empty");

  item_ptr item = iview.front();

  UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

  item = iview.back();
  
  UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");
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
  UNIT_ASSERT_FALSE((int)ostore_.empty(), "object store shouldn't be empty");

  ostore_.clear();

  UNIT_ASSERT_TRUE((int)ostore_.empty(), "object store must be empty");
  UNIT_ASSERT_EQUAL((int)iview.size(), 0, "invalid item view size");
  UNIT_ASSERT_TRUE((int)iview.empty(), "item view must be empty");

  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();

  UNIT_ASSERT_FALSE(first == last, "prototype iterator shouldn't be the same");

  ostore_.clear(true);

  first = ostore_.begin();
  last = ostore_.end();

  UNIT_ASSERT_TRUE(++first == last, "prototype iterator must be the same");
}

void
ObjectStoreTestUnit::generic_test()
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
  const char *cstr("baba");
  std::string title = "Hallo Welt";
  oos::varchar<64> str("The answer is 42");

  Item *item = new Item();
  
  item->set("val_char", c);
  item->set("val_float", f);
  item->set("val_double", d);
  item->set("val_short", s);
  item->set("val_int", i);
  item->set("val_long", l);
  item->set("val_unsigned_short", us);
  item->set("val_unsigned_int", ui);
  item->set("val_unsigned_long", ul);
  item->set("val_bool", b);

  item->set("val_cstr", cstr);

  item->set("val_string", title);
  item->set("val_varchar", str);
  /* get float value into string
   * with precision 2
   */
  item->get("val_float", str, 2);
  UNIT_ASSERT_EQUAL(str, "1.55", "float string is invalid");

  item->get("val_int", str);
  UNIT_ASSERT_EQUAL(str, "-98765", "float string is invalid");

  item->get("val_double", str, 3);
  UNIT_ASSERT_EQUAL(str, "123.558", "double string is invalid");

  delete item;
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
}
