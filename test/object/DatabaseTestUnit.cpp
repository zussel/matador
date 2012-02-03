#include "DatabaseTestUnit.hpp"

#include "Car.hpp"
#include "Engine.hpp"
#include "Item.hpp"

#include "object/object_view.hpp"
#include "object/object_list.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"
#include "database/statement_helper.hpp"

#include <iostream>
#include <fstream>

using namespace oos;
using namespace std;

DatabaseTestUnit::DatabaseTestUnit()
  : unit_test("database test unit")
{
  add_test("helper", std::tr1::bind(&DatabaseTestUnit::helper, this), "test statement helper");
  add_test("simple", std::tr1::bind(&DatabaseTestUnit::simple, this), "simple database test");
  add_test("with_sub", std::tr1::bind(&DatabaseTestUnit::with_sub, this), "object with sub object database test");
  add_test("with_list", std::tr1::bind(&DatabaseTestUnit::with_list, this), "object with object list database test");
  add_test("test", std::tr1::bind(&DatabaseTestUnit::test, this), "simple test method");
}

DatabaseTestUnit::~DatabaseTestUnit()
{}

void
DatabaseTestUnit::initialize()
{
  ostore_.insert_prototype(new object_producer<Item<ItemPtrList> >, "ITEM_PTR");
  ostore_.insert_prototype(new object_producer<ItemPtrList>, "ITEM_PTR_LIST");

  ostore_.insert_prototype(new object_producer<Car>, "CAR");
  ostore_.insert_prototype(new object_producer<Engine>, "ENGINE");
}

void
DatabaseTestUnit::finalize()
{
  ostore_.clear();
}

void
DatabaseTestUnit::helper()
{
  statement_helper help;
  
  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();
  
  cout << endl;

  while (first != last) {
    const prototype_node &node = (*first++);

    if (node.abstract) {
      continue;
    }

    object *o = node.producer->create();
    
    std::string stmt = help.create(o, node.type, statement_helper::CREATE);
    cout << "statement: " << stmt << endl;
    
    stmt = help.create(o, node.type, statement_helper::SELECT);
    cout << "statement: " << stmt << endl;
    
    stmt = help.create(o, node.type, statement_helper::INSERT);
    cout << "statement: " << stmt << endl;
    
    stmt = help.create(o, node.type, statement_helper::UPDATE);
    cout << "statement: " << stmt << endl;
    
    stmt = help.create(o, node.type, statement_helper::DEL);
    cout << "statement: " << stmt << endl;
    
    stmt = help.create(o, node.type, statement_helper::DROP);
    cout << "statement: " << stmt << endl;
    
    cout << endl;

    delete o;
  }
}

void
DatabaseTestUnit::simple()
{
  cout << endl;
  // create database and make object store known to the database
  database db(ostore_, "sqlite://simple.sqlite");

  // load data
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();

    // ... do some object modifications
    typedef object_ptr<Engine> engine_ptr;
    typedef object_view<Engine> engine_view;
    // insert new object
    engine_ptr engine = ostore_.insert(new Engine(70, 4, 1.4f));
    cout << "inserted " << *engine << "\n";
    tr.commit();

    tr.begin();
    // modify object
    engine->power(120);
    cout << "changed power of " << *engine << "\n";
    
    transaction tr2(db);
    try {
      // begin inner transaction
      tr2.begin();
      // change name again
      engine->power(170);
      cout << "changed power of " << *engine << "\n";
      // rollback transaction
      tr2.rollback();
      cout << "after rollback " << *engine << "\n";
    } catch (exception &) {
      tr2.rollback();
    }
    tr.rollback();
    cout << "after rollback " << *engine << "\n";

    tr.begin();
    // delete object
    cout << "deleting " << *engine << "\n";
    ostore_.remove(engine);

    // show objects
    ostore_.dump_objects(cout);

    cout << "starting rollback \n";
    tr.rollback();
    cout << "finished rollback \n";
    cout << "after rollback \n";

    // show objects
    ostore_.dump_objects(cout);

    engine_view view(ostore_);
    if (view.size() > 0) {
      engine_view::iterator i = view.begin();
      cout << "got " << *i.optr() << endl;
    } else {
      cout << "ERROR: no object in engine view!\n";
    }

    cout << "number of engines: " << view.size() << "\n";

    tr.begin();
    // delete object
    engine = *view.begin();
    cout << "deleting " << *engine << " (id: " << engine->id() << ")\n";
    ostore_.remove(engine);
    
    tr.commit();

    cout << "number of engines: " << view.size() << "\n";
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
  database db(ostore_, "sqlite://with_sub.sqlite");

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
    typedef object_ptr<Car> car_ptr;
    typedef object_ptr<Engine> engine_ptr;
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
  // explicit write data to file
  //db->write();
}

void
DatabaseTestUnit::with_list()
{
  typedef object_ptr<ItemPtrList> itemlist_ptr;
  typedef object_ptr<ItemPtrList::value_type> item_ptr;

  cout << endl;
  ostore_.dump_objects(cout);
  // create database and make object store known to the database
  database db(ostore_, "sqlite://with_list.sqlite");

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
      cout << "item [" << i->get()->name() << "]\n";
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
      cout << "item [" << i->get()->name() << "] ";
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
      cout << "item [" << i->get()->name() << "] ";
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
DatabaseTestUnit::test()
{
  std::cout << std::endl;
  std::string str("hallo welt und alle anderen 12345678901234567890");
  std::cout << "sizeof str: " << sizeof(str) << "\n";

  std::tr1::shared_ptr<string> strp(new string("hallo welt und alle anderen 12345678901234567890"));
  std::cout << "sizeof strp: " << sizeof(strp) << "\n";
}
