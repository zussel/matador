#include "DatabaseTestUnit.hpp"

#include "Car.hpp"
#include "Engine.hpp"
#include "Item.hpp"

#include "object/object_view.hpp"
#include "object/object_list.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace oos;
using namespace std;

DatabaseTestUnit::DatabaseTestUnit()
  : unit_test("database test unit")
{
  add_test("open", std::tr1::bind(&DatabaseTestUnit::open, this), "open database test");
  add_test("simple", std::tr1::bind(&DatabaseTestUnit::simple, this), "simple database test");
  add_test("complex", std::tr1::bind(&DatabaseTestUnit::with_sub, this), "object with sub object database test");
  add_test("list", std::tr1::bind(&DatabaseTestUnit::with_list, this), "object with object list database test");
  add_test("vector", std::tr1::bind(&DatabaseTestUnit::with_vector, this), "object with object vector database test");
}

DatabaseTestUnit::~DatabaseTestUnit()
{}

void
DatabaseTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<ContainerItem<ItemPtrList> >, "ITEM_PTR");
  ostore_.insert_prototype(new object_producer<ItemPtrList>, "ITEM_PTR_LIST");

  ostore_.insert_prototype(new object_producer<Car>, "CAR");
  ostore_.insert_prototype(new object_producer<Engine>, "ENGINE");

  // delete db
  std::remove("test.sqlite");
}

void
DatabaseTestUnit::finalize()
{
  ostore_.clear();
}

void
DatabaseTestUnit::open()
{
  cout << endl;
  // create database and make object store known to the database
  database db(ostore_, "sqlite://test.sqlite");

  UNIT_ASSERT_TRUE(db.is_open(), "couldn't database open database");
}

void
DatabaseTestUnit::simple()
{
  cout << endl;
  // create database and make object store known to the database
  database db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load data
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();

    // ... do some object modifications
    typedef object_ptr<Item> item_ptr;
    typedef object_view<Item> item_view;
    // insert new object
    item_ptr item = ostore_.insert(new Item("Hello World", 70));
    cout << "inserted " << *item << "\n";
    tr.commit();

    tr.begin();
    // modify object
    item->set_int(120);
    cout << "changed int value of " << *item << "\n";
    
    transaction tr2(db);
    try {
      // begin inner transaction
      tr2.begin();
      // change name again
      item->set_int(170);
      cout << "changed int value of " << *item << "\n";
      // rollback transaction
      tr2.rollback();
      cout << "after rollback " << *item << "\n";
    } catch (exception &) {
      tr2.rollback();
    }
    tr.rollback();
    cout << "after rollback " << *item << "\n";

    tr.begin();
    // delete object
    cout << "deleting " << *item << "\n";
    ostore_.remove(item);

    // show objects
    ostore_.dump_objects(cout);

    cout << "starting rollback \n";
    tr.rollback();
    cout << "finished rollback \n";
    cout << "after rollback \n";

    // show objects
    ostore_.dump_objects(cout);

    item_view view(ostore_);
    if (view.size() > 0) {
      item_view::iterator i = view.begin();
      cout << "got " << *i.optr() << endl;
    } else {
      cout << "ERROR: no object in engine view!\n";
    }

    cout << "number of items: " << view.size() << "\n";

    tr.begin();
    // delete object
    item = *view.begin();
    cout << "deleting " << *item << " (id: " << item->id() << ")\n";
    ostore_.remove(item);
    
    tr.commit();

    cout << "number of items: " << view.size() << "\n";
  } catch (exception &ex) {
    // error, abort transaction
    cout << ex.what() << endl;
    tr.rollback();
  }
  // close db
  db.close();
}

void
DatabaseTestUnit::with_sub()
{
  cout << endl;
  ostore_.dump_objects(cout);
  // create database and make object store known to the database
  database db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load data
  /****************
   *
   * comment this statement and the following
   * will happen if data exists:
   * data won't be load
   * a car object with id 1 is created
   * and an exception on insert is thrown
   * because there is already a car object
   * with id 1
   ****************/
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    typedef object_ptr<ObjectItem<Item> > object_item_ptr;
    typedef object_ptr<Item> Item_ptr;
    // insert new object
    car_ptr car = ostore_.insert(new Car("VW", "Beetle", "A small old car, looking like a beetle."));    
    engine_ptr engine = car->engine();
    ostore_.dump_objects(cout);
    engine->power(120);
    engine->cylinder(4);
    engine->capacity(1.4f);
    cout << "inserted " << *car << " with " << *engine << endl;
    tr.commit();
    
    tr.begin();
    
    cout << "removing " << *car << " ... ";
    ostore_.remove(car);
    cout << "done.\n";
    ostore_.dump_objects(cout);
    cout << "starting rollback\n";
    tr.rollback();
    cout << "finished rollback\n";
    
    ostore_.dump_objects(cout);
    
  } catch (exception &ex) {
    // error, abort transaction
    cout << "caught exception: " << ex.what() << " (start rollback)\n";
    tr.rollback();
    ostore_.dump_objects(cout);
  }
  // close db
  db.close();
}

void
DatabaseTestUnit::with_list()
{
  typedef object_ptr<ItemPtrList> itemlist_ptr;
  typedef object_ptr<ItemPtrList::value_type> item_ptr;

  cout << endl;
  ostore_.dump_objects(cout);
  // create database and make object store known to the database
  database db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load db
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    cout << "inserting item pointer list\n";
    itemlist_ptr itemlist = ostore_.insert(new ItemPtrList);
    cout << "item list [" << itemlist->id() << "]\n";
    tr.commit();
    
    ostore_.dump_objects(cout);

    tr.begin();
    cout << "inserting 2 items\n";
    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new ItemPtrList::value_type(name.str()));
      itemlist->push_back(item);
    }

    cout << "items of list\n";
    for (ItemPtrList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
      cout << "item [" << i->get()->get_string() << "]\n";
    }
    cout << "list has " << itemlist->size() << " items\n";
    cout << "starting rollback\n";
    tr.rollback();
    cout << "finished rollback\n";
    cout << "list has " << itemlist->size() << " items\n";
    
    ostore_.dump_objects(cout);
    
    tr.begin();
    cout << "inserting 4 items\n";
    for (int i = 0; i < 4; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new ItemPtrList::value_type(name.str()));
      itemlist->push_back(item);
    }

    cout << "list has " << itemlist->size() << " items\n";
    cout << "items of list\n";
    for (ItemPtrList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
      cout << "item [" << i->get()->get_string() << "] ";
    }
    cout << endl;

    tr.commit();
    cout << "commited\n";
    
    ostore_.dump_objects(cout);

    tr.begin();
    
    cout << "start clearing list\n";
    itemlist->clear();
    cout << "finished clearing list\n";

    cout << "list has " << itemlist->size() << " items\n";
    cout << "starting rollback\n";
    tr.rollback();
    cout << "finished rollback\n";
    cout << "list has " << itemlist->size() << " items\n";
    cout << "items of list\n";
    for (ItemPtrList::const_iterator i = itemlist->begin(); i != itemlist->end(); ++i) {
      cout << "item [" << i->get()->get_string() << "] ";
    }
    cout << endl;

  } catch (exception &ex) {
    // error, abort transaction
    cout << "caught exception: " << ex.what() << " (start rollback)\n";
    tr.rollback();
    ostore_.dump_objects(cout);
  }
  // close db
  db.close();
  // explicit write data to file
  //db->write();
}

void
DatabaseTestUnit::with_vector()
{
}